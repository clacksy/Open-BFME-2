// cl: /O1 /MD /D_CRTIMP= /D_STLP_USE_STATIC_LIB /Ireference/shims/bfmealloc
// stlport
//
// ??4Rva00566695@@QAEAAV0@ABV0@@Z @0x00566695 37B.
// Copy assignment copying int at +4 and byte at +8 then assigning the
// map<int AsciiString> at +0xC through the rowed _Rb_tree assign at
// 0x00504496. Vptr at +0 is not copied. Caller at 0x00566959; unblocks
// 0x0056693D. Prev Rva00566663Copy same page same flags.
#include <map>

class AsciiString
{
public:
	AsciiString(const AsciiString &that);
	~AsciiString();
	AsciiString &operator=(const AsciiString &that);
private:
	void *m_text;
};

class Rva00566695
{
public:
	Rva00566695 &operator=(const Rva00566695 &that);
private:
	int m_04;
	char m_08;
public:
	virtual void dummy();
private:
	_STL::map<int, AsciiString> m_map;
};

Rva00566695 &Rva00566695::operator=(const Rva00566695 &that)
{
	m_04 = that.m_04;
	m_08 = that.m_08;
	m_map = that.m_map;
	return *this;
}

// ?Rva0056693DCopy@@YAPAVRva00566695@@PAV1@00@Z @0x0056693D 50B.
// Forward assign-copy loop stride 0x18 via rowed operator= at 0x00566695.
// Evidence: chain lane; callee rowed; caller 0x00566AAD; unblocks 0x00566A9A.
// Same 50B shape as rowed Rva004BA2E9Copy at 0x004BA2E9.
Rva00566695 *Rva0056693DCopy(Rva00566695 *first, Rva00566695 *last, Rva00566695 *out)
{
	int n = last - first;
	if (n <= 0)
		return out;
	for (int i = n; i != 0; --i) {
		*out = *first;
		++first;
		++out;
	}
	return out;
}
