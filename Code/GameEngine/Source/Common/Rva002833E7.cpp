// cl: /O1 /DNDEBUG /MD
//
// ?dup_002833e7@@YIXPAURva002833E7Holder@@@Z, retail 0x002833E7, 26 bytes.
//
// Owned-pointer reset. The member is read, then cleared BEFORE the guard, so
// the clear is unconditional and a re-entrant path cannot free twice; the old
// value is then destroyed and released.
//
//   2833e7: push esi
//           mov  esi,[ecx]          ; p = self->m_p
//           and  dword [ecx],0      ; self->m_p = 0   (3-byte and-zero, /O1)
//           test esi,esi
//           je   2833ff
//           mov  ecx,esi
//           call 0x0028279F         ; ~Rva0028279F  (non-virtual, thiscall)
//           push esi
//           call 0x002FD60          ; ??3@YAXPAX@Z, rowed scalar operator delete
//           pop  ecx
//   2833ff: pop  esi
//           ret
//
// The `call dtor / push p / call operator delete` pair is what `delete p`
// lowers to for a non-virtual destructor, which is why the body is written as
// a delete rather than a manual destructor call.
//
// Identity is NOT recovered. The packet offered
// `?insert@?$list@HV?$allocator@H@_STL@@@_STL@@QAE?AU?$_List_iterator@...` as a
// lead, but that is one of 55 candidates the sweep aligns equally well, and the
// packet itself warns "the first is not the answer". The shape is not a list
// insert at all: there is no node allocation, no link fixup and no iterator
// returned. Parked under its address per AGENTS.md.
//
// The callee at 0x0028279F is unrowed, so it is named from its address too.

struct Rva0028279F
{
	~Rva0028279F();
};

struct Rva002833E7Holder
{
	Rva0028279F *m_p; // +0x00
};

void __fastcall dup_002833e7(Rva002833E7Holder *self)
{
	Rva0028279F *p = self->m_p;
	self->m_p = 0;
	if (p)
	{
		delete p;
	}
}
