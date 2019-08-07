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
		uint16_t* body;
	};
	std::vector<FnumTableInfo> tablelist;
	typedef double (CFnumTable::*FREQFUNC)(double, int);
	double TuningFrequency;
	const uint16_t* CreateTable(FnumTableType type, double rate, int offset, FREQFUNC func);
	const uint16_t* GetFnumTable(int master, int devide, int offset);
	const uint16_t* GetTPTable(int master, int devide, int offset);
	const uint16_t* GetDeltaNTable(int master, int devide, int offset);
	const uint16_t* GetOPL4Table(int master, int devide, int offset);
	const uint16_t* GetSAATable(int master, int devide, int offset);
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
	const uint16_t* GetTable(FnumTableType type, int master, int devide, int offset);
	double GetOPMMasterOffset(int fsamp);

	void UpdateMasterTuning();
};

extern CFnumTable theFnum;