// cl: /O1 /MD
// ?rva005312BE@Rva005312BE@@QAEXXZ @ 0x005312BE (66B): __thiscall clears byte at +0x34 of each 0x44-sized entry.
// ?rva00531300@Rva005312BE@@QAEXXZ @ 0x00531300 (66B): same shape sets byte to 1.
// ?rva00531431@Rva005312BE@@QAEXHH@Z @ 0x00531431 (80B): bounded setter writes byte at +0x35 and flag at +0x1BA30.
// ?rva00531512@Rva005312BE@@QAEEHH@Z @ 0x00531512 (76B): bounded getter returns entry flag or 0.
// Offsets 0x1BA38/0x1BA3C/0x1BA40 shared with Rva005315B0IntPairField in Disp32IntPairFieldGetters.cpp.
// Callers at 0x002F960C 0x002FA743 0x002FB050 0x002FCA6F 0x002FD573. Owner unknown so honest address name.
class Rva00531132
{
public:
	void rva00531132(bool add, int value);
	int m_count;
	int m_items[12];
};
class Rva005312BEItem
{
public:
	Rva00531132 m_set;
	unsigned char m_cleared;
	unsigned char m_35;
	char m_pad1[0x44 - 0x34 - 2];
};
class Rva005312BE
{
public:
	void rva005312BE();
	void rva00531300();
	void rva00531431(int a, int b);
	unsigned char rva00531512(int a, int b);
	void rva00531342(struct Rva005312BERect *r);
	void rva0053155E(int a, int b, bool add, int value);
	char m_pad[0x1BA30];
	unsigned char m_flag1BA30;
	char m_pad2[0x1BA38 - 0x1BA30 - 1];
	Rva005312BEItem **m_ppItems;
	int m_outer;
	int m_inner;
};
struct Rva005312BERect
{
	int x0;
	int y0;
	int x1;
	int y1;
};
void Rva005312BE::rva005312BE()
{
	for (int i = 0; i < m_outer; ++i)
	{
		for (int j = 0; j < m_inner; ++j)
			m_ppItems[i][j].m_cleared = 0;
	}
}
void Rva005312BE::rva00531300()
{
	for (int i = 0; i < m_outer; ++i)
	{
		for (int j = 0; j < m_inner; ++j)
			m_ppItems[i][j].m_cleared = 1;
	}
}
unsigned char Rva005312BE::rva00531512(int a, int b)
{
	if (a < 0 || b < 0)
		return 0;
	int i = a / 16;
	int j = b / 16;
	if (i >= m_outer || j >= m_inner)
		return 0;
	return m_ppItems[i][j].m_cleared;
}
void Rva005312BE::rva00531431(int a, int b)
{
	m_flag1BA30 = 1;
	if (a < 0 || b < 0)
		return;
	int i = a / 16;
	int j = b / 16;
	if (i >= m_outer || j >= m_inner)
		return;
	m_ppItems[i][j].m_35 = 1;
}

void Rva005312BE::rva00531342(Rva005312BERect *r)
{
	if (r->x1 < r->x0)
		return;
	if (r->y1 < r->y0)
		return;
	if (r->x1 < 0 || r->y1 < 0)
		return;
	if (r->x0 >= m_outer * 16 || r->y0 >= m_inner * 16)
		return;
	m_flag1BA30 = 1;
	int x0 = r->x0 < 0 ? 0 : r->x0 / 16;
	int y0 = r->y0 < 0 ? 0 : r->y0 / 16;
	int x1 = r->x1 / 16;
	int y1 = r->y1 / 16;
	if (x1 >= m_outer)
		x1 = m_outer - 1;
	if (y1 >= m_inner)
		y1 = m_inner - 1;
	for (int i = x0; i <= x1; ++i)
	{
		for (int j = y0; j <= y1; ++j)
			m_ppItems[i][j].m_35 = 1;
	}
}
// ?rva0053155E@Rva005312BE@@QAEXHH_NH@Z @ 0x0053155E (82B): bounded forward to Rva00531132 set; callers at 0x002E9033 0x002E9090.
void Rva005312BE::rva0053155E(int a, int b, bool add, int value)
{
	if (a < 0 || b < 0)
		return;
	int i = a / 16;
	int j = b / 16;
	if (i >= m_outer || j >= m_inner)
		return;
	m_ppItems[i][j].m_set.rva00531132(add, value);
}
