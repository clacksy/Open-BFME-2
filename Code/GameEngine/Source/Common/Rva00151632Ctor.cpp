// cl: /O1 /GX /MD /DNDEBUG /DWIN32 /D_WINDOWS
//
// ??0Rva00151632@@QAE@PBD@Z, retail 0x001515CA, 98 bytes. Prototype ctor for
// the vtable at 0x007D3A6C (rowed ??1Rva00151632 at 0x00151632 plus deleting
// dtor at 0x00151717 prove the class): builds the GenBase009EB7D0 base via
// its 0x0061ED40 row, nulls the +0x14 link, constructs the +0x18 AsciiString
// from the name via the rowed StringBase<char> PBD ctor 0x00037BA0, then
// re-names non-empty entries via rowed AsciiString::format 0x00038150 with
// "fxShader_%08x" and the object address. Called from 0x001516C1. Layout
// mirrors the rowed dtor TU; twin precedent Rva0017FB41Ctor.

template <typename T>
class StringBase
{
	friend class AsciiString;

public:
	bool isEmpty() const;

protected:
	~StringBase();

	void *m_data;

private:
	StringBase(const char *str);
};

class AsciiString : public StringBase<char>
{
public:
	AsciiString(const char *str) : StringBase<char>(str) {}
	~AsciiString();
	void __cdecl format(const char *format, ...);
};

class GenBase009EB7D0
{
public:
	GenBase009EB7D0();
	virtual ~GenBase009EB7D0();
};

class Rva00151632 : public GenBase009EB7D0
{
public:
	Rva00151632(const char *name);

	char m_pad04[0x10]; // +0x04..+0x13, untouched by this body
	void *m_link; // +0x14
	AsciiString m_name; // +0x18
};

Rva00151632::Rva00151632(const char *name)
	: m_link(0), m_name(name)
{
	if (m_name.isEmpty())
		m_name.format("fxShader_%08x", this);
}
