#pragma once
#include <vector>

enum FnumTableType {
	none = 0,
	Fnumber = 1,
	TonePeriod = 2,
	DeltaN = 3,
	opl4 = 4,
};

class CFnumTable {
protected:
	struct FnumTableInfo {
		int rate;
		FnumTableType type;	// 1=Fnumber, 2=TonePeriod, 3=DeltaN
		UINT16* body;
	};
	std::vector<FnumTableInfo> tablelist;
public:
	CFnumTable();
	~CFnumTable();
	const UINT16* GetTable(FnumTableType type, int master, int devide, int offset);
	const UINT16* GetFnumTable(int master, int devide, int offset);
	const UINT16* GetTPTable(int master, int devide, int offset);
	const UINT16* GetDeltaNTable(int master, int devide, int offset);
	const UINT16* GetOPL4Table(int master, int devide, int offset);
};

extern CFnumTable theFnum;