// ?rva0044770F@GameSlot@@QBE_NXZ
// partial score=0.98 date=2026-09-29
// ?rva0044770F@GameSlot@@QBE_NXZ
// partial score=0.98 date=2026-09-29
// ?rva0044770F@GameSlot@@QBE_NXZ
// partial score=0.98 date=2026-09-29
// cl: /O1 /G7 /Oy- /DNDEBUG /MD
//
// ?rva0044770F@GameSlot@@QBE_NXZ, retail 0x0044770F, 100 bytes.
// Chain via BfmeNetAddress compare 0x00248CBF plus GameSlot isHuman 0x003FF0F1
// plus null-checked global 0x009FE958 slot 64 returning address plus embedded
// address at GameSlot+0x38 plus port bump by 8 on the copied address.

typedef unsigned int UnsignedInt;
typedef unsigned short UnsignedShort;
typedef unsigned char UnsignedByte;
typedef int Int;
typedef bool Bool;

struct BfmeNetAddress
{
	bool Rva00248CBF(const BfmeNetAddress *other) const;
	UnsignedInt ip;
	UnsignedShort port;
};

struct Global009FE958
{
	virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
	virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
	virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
	virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
	virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
	virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
	virtual int v24(); virtual int v25(); virtual int v26(); virtual int v27();
	virtual int v28(); virtual int v29(); virtual int v30(); virtual int v31();
	virtual int v32(); virtual int v33(); virtual int v34(); virtual int v35();
	virtual int v36(); virtual int v37(); virtual int v38(); virtual int v39();
	virtual int v40(); virtual int v41(); virtual int v42(); virtual int v43();
	virtual int v44(); virtual int v45(); virtual int v46(); virtual int v47();
	virtual int v48(); virtual int v49(); virtual int v50(); virtual int v51();
	virtual int v52(); virtual int v53(); virtual int v54(); virtual int v55();
	virtual int v56(); virtual int v57(); virtual int v58(); virtual int v59();
	virtual int v60(); virtual int v61(); virtual int v62(); virtual int v63();
	virtual BfmeNetAddress *v64();
};
#define TheGlobal009FE958 (*(Global009FE958 **)0x009FE958)

class GameSlot
{
public:
	Bool isHuman() const;
	Bool rva0044770F() const;

private:
	void *m_vtable;
	Int m_state;
	Bool m_isAccepted;
	Bool m_hasMap;
	Bool m_isMuted;
	char m_pad0B[1];
	Int m_color;
	Int m_startPos;
	Int m_bfme14;
	Int m_playerTemplate;
	Int m_teamNumber;
	Int m_bfme20;
	Int m_origColor;
	Int m_origStartPos;
	Int m_origPlayerTemplate;
	char m_pad30[8];
	BfmeNetAddress m_addr38;
};

// ?rva0044770F@GameSlot@@QBE_NXZ present-unmatched
Bool GameSlot::rva0044770F() const
{
	if (!isHuman())
		return false;
	Global009FE958 *g = TheGlobal009FE958;
	if (g != 0)
	{
		const BfmeNetAddress *mine = &m_addr38;
		if (g->v64()->Rva00248CBF(mine))
			return true;
		BfmeNetAddress tmp = *TheGlobal009FE958->v64();
		tmp.port += 8;
		return tmp.Rva00248CBF(mine);
	}
	return false;
}
