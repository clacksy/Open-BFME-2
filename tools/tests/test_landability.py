#!/usr/bin/env python3
"""The landability verdict, on owned inputs rather than the live queue.

The queue moves every few minutes as the fleet lands rows, so a measurement
pinned to it is not a measurement. These cases are the three deficits the tool
exists to separate -- unproven boundary, unrowed callee, compiler shape -- each
written as the packet that produced it (0x00203E47, 0x005E16DA, 0x0075B948).
"""
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))

import landability  # noqa: E402

SNAPSHOT = "?Snapshot@@QAE@XZ"
# The real 0x005E16DA line, after the rebasing fix: one call to an address no
# symbol resolves to, while the ledger holds the name at 0x53A6.
UNPINNED = f"{SNAPSHOT},0x00221635 (unpinned: this is the address retail calls; " \
           "the ledger holds this name at 0x000053A6)"
# The pre-fix line, which read the candidate's offsets against the corrected
# start. It must still be counted, not dropped.
CORRUPT = f"{SNAPSHOT},0x-16B05E16 (unpinned: this is the address retail calls)"
ROWED = "?callee@Thing@@QAEXXZ,0x00200000 (already in the ledger)"
PINNED = "?callee@Thing@@QAEXXZ,0x00240000 (already pinned in reverse/symbols.csv)"


def test_a_packet_that_aligns_well_and_has_a_ghidra_start_is_only_one_pin_away():
    result = landability.verdict("??0LaserUpdateModuleData@@QAE@XZ", 35,
                                 True, "ghidra-start", UNPINNED)
    assert result["rank"] == landability.NEEDS_ONE_PIN
    assert result["boundary_proven"] is True
    assert [address for _, address in result["unresolved_callees"]] == [0x221635]
    assert landability.line(result) == (
        "needs-one-pin — Ghidra boundary, 1 callee needs a pin: 0x00221635")


def test_a_body_with_every_callee_rowed_is_landable():
    result = landability.verdict("?fn@Thing@@QAEXXZ", 20, True, "ghidra-start",
                                 "\n".join((ROWED, PINNED)))
    assert result["rank"] == landability.LANDABLE
    assert result["unresolved"] == 0
    assert landability.line(result).endswith("every callee rowed")


def test_two_unrowed_callees_are_not_dressed_up_as_one_pin():
    result = landability.verdict("?fn@Thing@@QAEXXZ", 40, True, "ghidra-start",
                                 "\n".join((UNPINNED, ROWED, PINNED, CORRUPT)))
    assert result["rank"] == landability.NEEDS_PINS
    assert result["unresolved"] == 2


def test_a_missing_inventory_row_is_unknown_not_wrong_but_still_a_deficit():
    """0x00203E47's shape: 92% alignment, no Ghidra row, no relative calls. The
    boundary is unproven, so the body cannot be served as landable however good
    the alignment reads -- three seats were spent on it."""
    result = landability.verdict("?iterate@TeamPrototype@@QBEXXZ", 30, None,
                                 "unmapped-gap", "(no relative calls in this body)")
    assert result["rank"] == landability.UNPROVEN_BOUNDARY
    assert result["boundary_proven"] is False
    assert "no function at this address" in landability.line(result)


def test_compiler_machinery_ranks_worst_whatever_else_is_true():
    """0075B948 and 007A8D18 were served as 4- and 1-byte bodies from a stray
    alignment hit; no C++ reproduces a $L label or an Unwind@ record."""
    local = landability.verdict("$L71877", 4, None, "unmapped-gap", "(no relative calls)")
    unwind = landability.verdict("Unwind@9e16da", 35, True, "ghidra-start",
                                 "(no relative calls)")
    for result in (local, unwind):
        assert result["rank"] == landability.SUSPECT_SHAPE
        assert result["rank"] > landability.UNPROVEN_BOUNDARY
    assert "local label" in local["detail"]
    assert "EH machinery" in unwind["detail"]


def test_a_malformed_pin_line_is_counted_not_dropped():
    """The pre-fix 0x005E16DA line wrote a negative address. Dropping an
    unparsable line would understate the deficit and promote the body."""
    parsed = landability.parse_pins(CORRUPT)
    assert parsed == [(CORRUPT, None, False)]
    result = landability.verdict("?fn@Thing@@QAEXXZ", 35, True, "ghidra-start", CORRUPT)
    assert result["rank"] == landability.NEEDS_ONE_PIN
    assert "unreadable" in landability.line(result)


def test_the_ledger_is_re_asked_so_a_pin_that_landed_after_generation_counts():
    """The packet's mark is the ledger as of the last `zh_sweep.py packets`, and
    rows land under it. A body whose dependency has since been pinned is
    landable, not one-pin-away."""
    stale = landability.verdict("?fn@Thing@@QAEXXZ", 35, True, "ghidra-start", UNPINNED)
    assert stale["rank"] == landability.NEEDS_ONE_PIN
    fresh = landability.verdict("?fn@Thing@@QAEXXZ", 35, True, "ghidra-start", UNPINNED,
                                resolves=lambda symbol, address:
                                symbol == SNAPSHOT and address == 0x221635)
    assert fresh["rank"] == landability.LANDABLE


def test_band_serves_the_best_evidence_and_says_what_it_set_aside():
    landable = {"function": "a", "landability": {"rank": landability.LANDABLE}}
    one_pin = {"function": "b", "landability": {"rank": landability.NEEDS_ONE_PIN}}
    unproven = {"function": "c", "landability": {"rank": landability.UNPROVEN_BOUNDARY}}
    kept, set_aside = landability.band([unproven, one_pin, landable])
    assert [c["function"] for c in kept] == ["a"]
    assert set_aside == 2

    kept, set_aside = landability.band([unproven, one_pin])
    assert [c["function"] for c in kept] == ["b"]
    assert set_aside == 1


def test_band_leaves_unscored_candidates_alone():
    """Only the packet tier is scored; a queue with no verdicts must pass
    through untouched rather than being filtered to nothing."""
    plain = [{"function": "a"}, {"function": "b"}]
    kept, set_aside = landability.band(plain)
    assert kept == plain and set_aside == 0


def test_only_a_boundary_proven_packet_preempts_the_other_tiers():
    """Once 0x005E16DA landed, the packet queue's best remaining band is
    0x00203E47 at unproven-boundary. Drawing that by default would put it ahead
    of the named/structural/ghidra tiers, whose addresses are at least
    validated, so the scored default pick would be worse than the unscored one.
    An explicit --tier packet still serves it."""
    def band_of(rank):
        return [{"function": "x", "landability": {"rank": rank}}]

    assert landability.preempts(band_of(landability.LANDABLE))
    assert landability.preempts(band_of(landability.NEEDS_ONE_PIN))
    assert landability.preempts(band_of(landability.NEEDS_PINS))
    assert not landability.preempts(band_of(landability.UNPROVEN_BOUNDARY))
    assert not landability.preempts(band_of(landability.SUSPECT_SHAPE))
    assert not landability.preempts([])
