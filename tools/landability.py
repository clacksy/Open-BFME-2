#!/usr/bin/env python3
"""Score a queued body on the deficits that actually predicted failure.

The queues rank by package alignment and by size, and neither measures whether a
body can be landed. Measured over the session that produced 0x002833E7 and
0x00262BEC: 2 of roughly 15 probed candidates landed, and both had a Ghidra
boundary row and every REL32 callee already rowed. Every failure had one of
three deficits:

1. an unproven boundary -- 0x00203E47 burned three seats before the packet's own
   header contradicted Ghidra;
2. an unrowed dependency -- a callee or global needing a new pin
   (0x005E16DA, getPlayerInfo);
3. a codegen shape invisible to a byte diff -- store order, cmp x2 vs dec/sub,
   immediate vs register, tail-call vs normalise.

So this module reports the three deficits instead of a distance:

  boundary   the Ghidra inventory confirms a function starts here, refuses it,
             or has no opinion. Absence is unknown, not wrong -- 61% of the
             bodies already landed are missing from the inventory.
  callees    how many REL32 targets no symbol resolves to, taken from the
             packet's own Callee-pins block, which is address-aware (retail
             folds and duplicates bodies, so a name the ledger holds is
             routinely called at a copy no row covers).
  shape      compiler machinery no C++ reproduces: an Unwind@/Catch@ funclet, a
             $L local label, or a body too short to hold an instruction.

The result is a rank, not a probability. The three deficits are not commensurable
and only the first two have been measured; inventing a weight for the third would
be exactly the unmeasured tuning tools/yield_model.py warns against.
"""
import re

# Ranks, best first. A caller sorts ascending and serves the best band present.
LANDABLE = 0
NEEDS_ONE_PIN = 1
NEEDS_PINS = 2
UNPROVEN_BOUNDARY = 3
SUSPECT_SHAPE = 4

LABELS = {
    LANDABLE: "landable",
    NEEDS_ONE_PIN: "needs-one-pin",
    NEEDS_PINS: "needs-pins",
    UNPROVEN_BOUNDARY: "unproven-boundary",
    SUSPECT_SHAPE: "suspect-shape",
}

# Ghidra names its SEH residue `Unwind@<va>` / `Catch@<va>`; tools/gen_uw.py owns
# that lane and tools/zh_sweep.py already skips them, so seeing one here means a
# packet was generated before that skip.
FUNCLET_PREFIXES = ("Unwind@", "Catch@")
# MSVC's own local labels: the tail of a function, an EH handler fragment or a
# switch table. `$L71877` and `$L35600` were served as 4B and 1B "bodies".
LOCAL_LABEL = re.compile(r"\$[A-Za-z_]+\d+$")
# Below this there is no room for an instruction and a return; the two local
# labels above were the whole of their packet.
MIN_PORTABLE = 5

# `symbol,0x00123456 (mark)`. The address is written 0x%08X by zh_sweep, so the
# eight digits are what separates the name from its address; a mangled MSVC name
# keeps its commas, which is why this cannot split on the first one. A negative
# or short address does not match, and is reported rather than trusted.
PIN_LINE = re.compile(r"^(?P<symbol>.+?),0x(?P<address>[0-9A-Fa-f]{8})(?![0-9A-Fa-f])"
                      r"(?P<mark>.*)$")
# The only two marks that mean the call site already resolves. Everything else --
# a bare `symbol,0xADDR`, an `(unpinned: ...)` note, a malformed address -- needs
# a pin before the body can pass the byte gate.
RESOLVED_MARKS = ("(already in the ledger)", "(already pinned in reverse/symbols.csv)")
NO_CALLS = "(no relative calls in this body)"


def parse_pins(pins_text):
    """[(symbol, address_or_None, resolved)] for one packet's Callee-pins block.

    An unparsable line is kept with `address=None` and `resolved=False`: a line
    zh_sweep wrote is evidence that a REL32 site exists, so silently dropping it
    would understate the deficit and could promote a body to `landable`.
    """
    found = []
    for raw in pins_text.splitlines():
        line = raw.strip()
        if not line or line == NO_CALLS:
            continue
        match = PIN_LINE.match(line)
        if not match:
            found.append((line, None, False))
            continue
        mark = match.group("mark").strip()
        found.append((match.group("symbol"), int(match.group("address"), 16),
                      mark in RESOLVED_MARKS))
    return found


def shape_complaint(function, size):
    """Why nothing here is a C++ body, or None. Positive evidence only.

    Every check names a shape the compiler emitted and no source author wrote.
    A plain `ret` tail is deliberately not consulted: a body that ends in
    something else is a tail call as often as it is a boundary error, which is
    the rule boundary_validator already follows.
    """
    if any(function.startswith(prefix) for prefix in FUNCLET_PREFIXES):
        return f"{function.split('@', 1)[0]} is compiler EH machinery"
    if LOCAL_LABEL.search(function):
        return f"{function} is a compiler local label, not a function start"
    if size < MIN_PORTABLE:
        return f"{size}B is too short to hold an instruction and a return"
    return None


def verdict(function, size, start_ok, start_why, pins_text, resolves=None):
    """The rank and the sentence that explains it for one address.

    `start_ok`/`start_why` come from boundary_validator.check_start. `resolves`,
    when given, re-asks the live symbol map about each parsed callee; without it
    the packet's own mark is trusted, which is the state of the ledger as of the
    last `zh_sweep.py packets` run. Either way the count is the packet's, never a
    re-derived guess.
    """
    pins = parse_pins(pins_text)
    if resolves is not None:
        pins = [(symbol, address, resolved or (address is not None and resolves(symbol, address)))
                for symbol, address, resolved in pins]
    unresolved = [(symbol, address) for symbol, address, resolved in pins if not resolved]

    shape = shape_complaint(function, size)
    if shape:
        return _verdict(SUSPECT_SHAPE, function, size, shape, unresolved, start_ok, start_why)
    if start_ok is not True:
        detail = ("the Ghidra inventory has no function at this address"
                  if start_ok is None else f"the inventory refuses it ({start_why})")
        return _verdict(UNPROVEN_BOUNDARY, function, size, detail, unresolved, start_ok, start_why)
    rank = LANDABLE if not unresolved else (NEEDS_ONE_PIN if len(unresolved) == 1
                                            else NEEDS_PINS)
    return _verdict(rank, function, size, None, unresolved, start_ok, start_why)


def _verdict(rank, function, size, detail, unresolved, start_ok, start_why):
    return {"rank": rank, "label": LABELS[rank], "detail": detail,
            "unresolved": len(unresolved), "unresolved_callees": unresolved,
            "boundary_proven": start_ok is True, "size": size, "function": function}


def line(result):
    """One line for the work queue: the rank, the evidence, and the deficit."""
    parts = []
    if result["boundary_proven"]:
        parts.append("Ghidra boundary")
    if result["detail"]:
        parts.append(result["detail"])
    count = result["unresolved"]
    if count == 0:
        parts.append("every callee rowed")
    elif count == 1:
        parts.append(f"1 callee needs a pin: {_addresses(result)}")
    else:
        parts.append(f"{count} callees need pins: {_addresses(result)}")
    return f"{result['label']} — " + ", ".join(parts)


def _addresses(result):
    shown = [f"0x{address:08X}" if address is not None else "address unreadable"
             for _, address in result["unresolved_callees"][:3]]
    if result["unresolved"] > len(shown):
        shown.append(f"+{result['unresolved'] - len(shown)} more")
    return " ".join(shown)


def band(candidates):
    """The best landability band present, plus what it set aside.

    A filter, not a reweight, for the reason tools/next_work.py gives about
    deferrals: ordering is a scheduling rule and claims nothing about
    probability. A queue that offers a `landable` body must not spend a seat on
    an `unproven-boundary` one, however much larger the latter is -- that is the
    measured shape of the last session, where three seats went to 0x00203E47.
    """
    ranked = [c for c in candidates if c.get("landability")]
    if not ranked:
        return list(candidates), 0
    best = min(c["landability"]["rank"] for c in ranked)
    kept = [c for c in candidates if not c.get("landability")
            or c["landability"]["rank"] == best]
    return kept, len(candidates) - len(kept)


def preempts(kept_band):
    """Should this band displace a lower-priority tier in the default pick?

    Only when its boundary is proven. An unproven boundary is exactly the
    deficit that burned three seats on 0x00203E47, and the other tiers at least
    validate their addresses against the inventory, so a packet the inventory
    cannot place must never be drawn ahead of them -- it would make the default
    pick worse than it was before any of this was scored. An explicit
    `--tier packet` still serves it, deliberately.
    """
    return bool(kept_band) and kept_band[0]["landability"]["rank"] < UNPROVEN_BOUNDARY
