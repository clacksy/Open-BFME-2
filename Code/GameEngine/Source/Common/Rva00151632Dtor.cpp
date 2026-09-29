// cl: /O1 /GX /MD /DNDEBUG /DWIN32 /D_WINDOWS
//
// ??1Rva00151632@@UAE@XZ, retail 0x00151632, 90 bytes. Virtual dtor twin of
// Rva0017FB94 (89B) and Rva0014CD63 (82B): stores vtable 0x007D3A6C, deletes
// the +0x14 link via slot-0 virtual get(0) plus operator delete 0x0002FD60,
// tears down +0x18 AsciiString via pinned ??1AsciiString 0x00036410, then
// delegates to base pinned ??1Rva0061ED80 0x0061ED80 (same bytes as rowed
// ?bfmeResetUB). Called from deleting dtor 0x00151717. Layout mirrors the
// twins: vtable + pad 0x10 + link + String. EH via /GX like the twins.

class Rva00151632Link
{
public:
	virtual void *get(int x);
};

class AsciiString
{
public:
	~AsciiString();
};

class Rva0061ED80
{
public:
	virtual ~Rva0061ED80();
};

class Rva00151632 : public Rva0061ED80
{
public:
	virtual ~Rva00151632();

private:
	char m_pad04[0x10];
	Rva00151632Link *m_link;
	AsciiString m_name;
};

Rva00151632::~Rva00151632()
{
	void *tmp = m_link ? m_link->get(0) : 0;
	delete tmp;
}
