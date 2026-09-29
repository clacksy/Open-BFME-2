// cl: /O1 /MD
//
// ??_GRva00151632@@UAEPAXI@Z, retail 0x00151717, 28 bytes. Deleting dtor for
// Rva00151632 (vtable 0x007D3A6C slot 9): calls rowed ??1Rva00151632 90B at
// 0x00151632 then operator delete 0x0002FD60 on flag. Chain from 0x00151632.
// Public virtual (UAE) like Rva0017FB94 twin at 0x0017FD8B.

// ??_GRva00151632@@UAEPAXI@Z @0x151717
class Rva00151632 { public: __declspec(noinline) virtual ~Rva00151632(); private: int m_famgen; };
Rva00151632::~Rva00151632() { m_famgen = 0; }
void famgenDelete(Rva00151632 *p) { delete p; }
