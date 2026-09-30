// cl: /O1 /DNDEBUG /MD
//
// ?Rva00262BEC@Rva00262BECOwner@@QBE_NXZ, retail 0x00262BEC, 39 bytes.
//
// A const predicate: true only when a state word is 1 or 4 AND the object at
// +0x140 exists AND that object's own predicate holds.
//
//   262bec: mov  eax,[ecx+0x1fc]   ; state
//           dec  eax / je  L       ; state == 1
//           sub  eax,3 / jne out   ; state == 4
//     L:    mov  ecx,[ecx+0x140]   ; the sub-object
//           test ecx,ecx / je out
//           call 0x003638BA        ; ?rva003638BA@Rva003638BA@@QAE_NXZ, rowed
//           test al,al / je out
//           mov  al,1 / ret
//     out:  xor  al,al / ret
//
// THREE levers this body cost. The first two were visible in the byte diff and
// the third was not, so it is the one worth remembering:
//
//  1. `return m_140->f();` in a tail position tail-called the callee (`e9`).
//     Retail calls and then normalises (`test al,al`), so the value has to be
//     consumed rather than returned directly.
//  2. `return (a && b && c);` normalised correctly but materialised the true
//     value as `xor eax,eax / inc eax` (3 bytes). Retail's `mov al,1` (2 bytes)
//     is what a literal `return true;` produces.
//  3. `if (state == 1 || state == 4)` compiles to two compares, `cmp eax,1` and
//     `cmp eax,4`, which is one byte longer than retail's two-value chain. A
//     `switch` with those two cases is what makes MSVC emit the `dec eax` /
//     `sub eax,3` pairing. This is the only one of the three that a byte diff
//     cannot suggest -- both spellings are "the same test" to a reader.
//
// Boundary is proven: ghidra carries `0x262BEC,39,FUN_00662bec` and the address
// was unclaimed. The callee is already rowed at 0x003638BA, so no pin was added.
// Identity is NOT recovered: the name and the member offsets are
// address-derived, and the packet's InGameChat.cpp lead is unnamed.

class Rva003638BA
{
public:
	bool rva003638BA();
};

class Rva00262BECOwner
{
	char m_pad[0x140];
	Rva003638BA *m_140;                // +0x140
	char m_pad2[0x1FC - 0x144];
	int m_1fc;                         // +0x1fc

public:
	bool Rva00262BEC() const;
};

bool Rva00262BECOwner::Rva00262BEC() const
{
	switch (m_1fc)
	{
	case 1:
	case 4:
		if (m_140 != 0)
		{
			if (m_140->rva003638BA())
			{
				return true;
			}
		}
		break;

	default:
		break;
	}
	return false;
}
