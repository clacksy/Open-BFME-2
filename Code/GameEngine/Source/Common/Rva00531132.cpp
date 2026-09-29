// cl: /O1 /MD
// ?rva00531132@Rva00531132@@QAEX_NH@Z @ 0x00531132 (77B): __thiscall add/remove int in 12-entry set; caller at 0x005315A5.
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
class Rva00531132
{
public:
	void rva00531132(bool add, int value);
	int m_count;
	int m_items[12];
};
void Rva00531132::rva00531132(bool add, int value)
{
	if (value == 0)
		return;
	int count = m_count;
	int index = 0;
	if (count > 0)
	{
		int *p = m_items;
		do
		{
			if (*p == value)
				break;
			++index;
			++p;
			_ReadWriteBarrier();
		} while (index < m_count);
	}
	if (add)
	{
		if (index < count)
			return;
		if (count >= 12)
			return;
		m_items[count] = value;
		++m_count;
	}
	else
	{
		if (index >= count)
			return;
		int last = count - 1;
		m_count = last;
		m_items[index] = m_items[last];
	}
}
