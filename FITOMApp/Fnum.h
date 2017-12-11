#pragma once
#include <vector>

enum FnumTableType {
	none = 0,
	Fnumber = 1,
	TonePeriod = 2,
	DeltaN = 3,
	opl4 = 4,
	saa = 5,
};

class CFnumTable {
protected:
	struct FnumTableInfo {
		double rate;
		int offset;
		FnumTableType type;	// 1=Fnumber, 2=TonePeriod, 3=DeltaN
		UINT16* body;
	};
	std::vector<FnumTableInfo> tablelist;
	typedef double (CFnumTable::*FREQFUNC)(double, int);
	double TuningFrequency;
	const UINT16* CreateTable(FnumTableType type, double rate, int offset, FREQFUNC func);
	const UINT16* GetFnumTable(int master, int devide, int offset);
	const UINT16* GetTPTable(int master, int devide, int offset);
	const UINT16* GetDeltaNTable(int master, int devide, int offset);
	const UINT16* GetOPL4Table(int master, int devide, int offset);
	const UINT16* GetSAATable(int master, int devide, int offset);
	FREQFUNC GetFreqFunc(FnumTableType type);
	double GetFnum(double rate, int note);
	double GetTP(double rate, int note);
	double GetDeltaN(double rate, int note);
	double GetOPL4Fnum(double rate, int note);
	double GetSAATP(double rate, int note);
public:
	CFnumTable();
	~CFnumTable();
	void SetMasterPitch(double pitch) { TuningFrequency = pitch; };
	const UINT16* GetTable(FnumTableType type, int master, int devide, int offset);
	double GetOPMMasterOffset(int fsamp);

	void UpdateMasterTuning();
};

extern CFnumTable theFnum;