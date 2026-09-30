// ??0Rva005E16DA@@QAE@H@Z
// partial score=0.95 date=2026-09-30
// cl: /O1 /DNDEBUG /MD
// PARTIAL -- not landed. Both shapes are exactly 35 bytes; the ONLY residual
// is store order. Retail installs the vptr before the member zeroes:
//
//   target:   56 ff 74 24 08 8b f1 e8 4f ff c3 ff | 33 c0 | c7 06 <vtable> |
//             89 46 08 | 89 46 0c | 89 46 10 | 8b c6 5e c2 04 00
//   compiled: 56 ff 74 24 08 8b f1 e8 4f ff c3 ff | 33 c0 |
//             89 46 08 | 89 46 0c | 89 46 10 | c7 06 <vtable> | 8b c6 5e c2 04 00
//
// Tried and REFUTED as levers for the order: member assignments in the
// constructor body, and a mem-initializer list -- MSVC emits the vptr store
// last under /O1 either way. The base ctor call, the `xor eax,eax`, every
// displacement and the `ret 4` all match already.
//
// Established: the base ctor at 0x00221635 is unrowed and pinned address-derived
// (its target decays from retail's own call: 0x5E16E1 + 5 - 3932337 = 0x221635).
// The vtable store is a masked DIR32, so the vtable's contents never enter the
// comparison -- a PURE virtual is enough to make the compiler emit one without
// adding a second function definition to the TU.
//
//
// ?Rva005E16DA@Rva005E16DA@@QAE@H@Z, retail 0x005E16DA, 35 bytes.
//
// Derived constructor. `this` in ecx, one stack argument, ret 4:
//
//   5e16da: push esi
//           push [esp+8]        ; the argument, at its post-push slot
//           mov  esi,ecx
//           call 0x00221635     ; base ctor, same argument
//           xor  eax,eax
//           mov  [esi],0x00C77A30   ; derived vtable (DIR32, masked)
//           mov  [esi+8],eax / [esi+0xc],eax / [esi+0x10],eax
//           mov  eax,esi
//           pop  esi
//           ret  4
//
// The base owns the vptr at +0 plus one dword at +4, which is why the derived
// members begin at +8. The vtable is declared PURE so the compiler emits one
// without requiring a second function definition in this TU: an abstract
// class still gets a vtable and its constructor still installs it, and the
// vtable's contents never enter the byte comparison because the store is a
// masked DIR32.
//
// The base ctor at 0x00221635 is unrowed, so it is pinned from its address.
// Its target is established by decaying retail's own call, not from the shape
// this TU wants.
//
// Boundary is proven: ghidra carries `0x5E16DA,35,FUN_009e16da` and the address
// was unclaimed. Identity is NOT recovered; the packet's LaserUpdate.cpp lead
// is unnamed.

class Rva00221635Base
{
public:
	virtual void Rva00221635Base_pure() = 0; // vptr at +0
	int m_4;                                 // +0x04

	Rva00221635Base(int arg);
};

class Rva005E16DA : public Rva00221635Base
{
	int m_8;   // +0x08
	int m_c;   // +0x0c
	int m_10;  // +0x10

public:
	Rva005E16DA(int arg);
	virtual void Rva005E16DA_pure() = 0;
};

Rva005E16DA::Rva005E16DA(int arg)
	: Rva00221635Base(arg), m_8(0), m_c(0), m_10(0)
{
}
