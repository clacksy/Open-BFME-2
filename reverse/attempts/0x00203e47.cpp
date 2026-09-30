// ?Rva00203E47@Rva00203E47Owner@@QAEXPAURva00203E47Out@@@Z
// partial score=0.7 date=2026-09-30
// cl: /O1 /DNDEBUG /MD
//
// ?Rva00203E47@Rva00203E47Owner@@QAEXPAURva00203E47Out@@@Z
// retail 0x00203E47, 30 bytes. PARTIAL -- not landed.
//
// This is the closer of the two shapes tried. It is byte-identical to retail
// from the first instruction through `mov [eax],ecx`, and differs only in how
// the two constants are materialised.
//
//   target:   8b 44 24 04 | 8b 89 34 03 00 00 | 56 | 33 f6 | ba f5 4a 9c 00 |
//             89 70 0c | 89 08 | 89 50 08 | 5e | c2 04 00
//   compiled: 8b 44 24 04 | 83 60 0c 00 | 8b 89 34 03 00 00 | 89 08 |
//             c7 40 08 <imm32> | c2 04 00
//
// REMAINING WALL -- one lever, and it is instruction selection, not shape:
// retail materialises BOTH constants into registers and stores from them
// (`xor esi,esi` / `mov edx,0x009C4AF5`, then `mov [eax+0xc],esi` and
// `mov [eax+8],edx`). This source lets MSVC fold both to immediates instead
// (`and dword [eax+0xc],0` and `mov dword [eax+8],imm32`), which also removes
// the esi save/restore pair and shifts every later offset by 3 and 7 bytes.
//
// REFUTED this round, so the next seat does not respend them (each still emits
// a 30B body that folds both constants):
//   * /O2 and /Ox: the zero becomes the 7-byte `mov dword [eax+0xc],0` and the
//     string stays an immediate, so the TU is not merely at the wrong level.
//   * Source order f0,f8,fC and f8,fC,f0.
//   * Locals (`unsigned z = 0; const char *e = "";`) then stores.
//   * Storing through a local `Out *o = out;`.
//   * fC typed as `void*` and set to null.
//
// BOUNDARY EVIDENCE gathered this round -- the other half of the deficit three
// earlier seats stopped on:
//   * The PRECEDING Ghidra inventory row is `0x00203E2C +27`, ending exactly at
//     0x203E47, and it is a matched ledger row (?Rva00203E2CXfer, same size).
//   * The claimed body ends exactly on the `ret 4` at 0x203E65, and the NEXT
//     function starts immediately there with no padding (`mov ecx,[0xdfe78c] /
//     call 0x1dcd1c / neg al / sbb eax,eax / inc eax / ret`), followed by a
//     scalar deleting destructor at 0x203E76.
//   * boundary_validator.check_end(0x203E47, 30) passes: no known function
//     start and no int3 run inside the range.
//   * No rowed body calls it directly (tools/callers_of.py). Absence there is
//     not evidence -- that lane covers a fraction of .text -- but the point is
//     that no xref proof exists either way. Both 0x203E65 and 0x203E76 are also
//     absent from the inventory, so this is a run of uninventoried functions
//     bracketed by known ones at both ends.
//
// What is already established, so the next seat does not redo it:
//   * No relative calls at all, so no callee pin is needed.
//   * 0x009C4AF5 is 48 zero bytes: the shared empty-string literal, so `""` is
//     the correct spelling and the reference rides as a relocation.
//   * +0x04 is never written, so it is an untouched field. An aggregate
//     `{ m_334, 0, "", 0 }` writes it and breaks the match.
//   * A return-by-value (hidden return pointer) form was tried and REFUTED: it
//     emits a full `push ebp / mov ebp,esp / sub esp,0x10` frame plus a copy of
//     a named local, where retail has no frame at all.
//   * So the signature is an explicit parameter with `ret 4`, and the store
//     order is +0xc, +0, +8.
//
// Identity is NOT recovered; the packet's Player.cpp lead is unnamed.

struct Rva00203E47Out
{
	const void *f0;  // +0x00
	int         f4;  // +0x04 -- deliberately not written by this body
	const char *f8;  // +0x08
	unsigned    fC;  // +0x0c
};

class Rva00203E47Owner
{
	char m_pad[0x334];
	const void *m_334; // +0x334

public:
	void Rva00203E47(Rva00203E47Out *out);
};

void Rva00203E47Owner::Rva00203E47(Rva00203E47Out *out)
{
	out->fC = 0;
	out->f0 = m_334;
	out->f8 = "";
}
