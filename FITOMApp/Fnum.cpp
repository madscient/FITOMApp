#include "STDAFX.H"
#include "Fnum.h"
#include <math.h>

CFnumTable::CFnumTable(double pitch) : TuningFrequency(pitch)
{
	tablelist.clear();
}

CFnumTable::~CFnumTable()
{
	for (std::vector<FnumTableInfo>::iterator iter = tablelist.begin(); iter != tablelist.end(); iter++) {
		delete[] iter->body;
	}
	tablelist.clear();
}

const UINT16* CFnumTable::GetTable(FnumTableType type, int master, int devide, int offset)
{
	if (!(master && devide)) {
		return 0;
	}
	switch (type) {
	case FnumTableType::Fnumber:
		return (GetFnumTable(master, devide, offset));
	case FnumTableType::DeltaN:
		return (GetDeltaNTable(master, devide, offset));
	case FnumTableType::TonePeriod:
		return (GetTPTable(master, devide, offset));
	case FnumTableType::opl4:
		return (GetOPL4Table(master, devide, offset));
	case FnumTableType::saa:
		return (GetSAATable(master, devide, offset));
	default:
		return 0;
	}
}


// Build or get F-number table (for FM)
const UINT16* CFnumTable::GetFnumTable(int master, int devide, int offset)
{
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (round(rate) == tablelist[i].rate && tablelist[i].type == FnumTableType::Fnumber) {
			return (const UINT16*)tablelist[i].body;
		}
	}
	FnumTableInfo fti;
	fti.rate = round(rate);
	fti.type = FnumTableType::Fnumber;
	fti.body = new UINT16[768];
	for (int i = 0; i < 768; i++) {
		double freq = TuningFrequency * pow(2.0, (double)(i + offset) / 768.0);
		double sig = round(freq * (pow(2, 17) / rate));
		fti.body[i] = UINT16(sig);
	}
	tablelist.push_back(fti);
	return GetFnumTable(master, devide, offset);
}

// Build or get Tone period table (for PSG)
const UINT16* CFnumTable::GetTPTable(int master, int devide, int offset)
{
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (round(rate) == tablelist[i].rate && tablelist[i].type == FnumTableType::TonePeriod) {
			return (const UINT16*)tablelist[i].body;
		}
	}
	FnumTableInfo fti;
	fti.rate = round(rate);
	fti.type = FnumTableType::TonePeriod;
	fti.body = new UINT16[768];
	for (int i = 0; i < 768; i++) {
		double freq = TuningFrequency * pow(2.0, (double)(i + offset) / 768.0);
		double sig = round((8 * rate) / freq);
		fti.body[i] = UINT16(sig);
	}
	tablelist.push_back(fti);
	return GetTPTable(master, devide, offset);
}

// Build or get Delta N table (for ADPCM)
const UINT16* CFnumTable::GetDeltaNTable(int master, int devide, int offset)
{
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (round(rate) == tablelist[i].rate && tablelist[i].type == FnumTableType::DeltaN) {
			return (const UINT16*)tablelist[i].body;
		}
	}
	FnumTableInfo fti;
	fti.rate = round(rate);
	fti.type = FnumTableType::DeltaN;
	fti.body = new UINT16[768];
	for (int i = 0; i < 768; i++) {
		double freq = 16000 * pow(2.0, (double)(i + offset) / 768.0);
		double sig = round(65536 * (freq / rate));
		fti.body[i] = UINT16(sig);
	}
	tablelist.push_back(fti);
	return GetDeltaNTable(master, devide, offset);
}

// Build or get F-number table (for OPL4)
const UINT16* CFnumTable::GetOPL4Table(int master, int devide, int offset)
{
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (round(rate) == tablelist[i].rate && tablelist[i].type == FnumTableType::opl4) {
			return (const UINT16*)tablelist[i].body;
		}
	}
	FnumTableInfo fti;
	fti.rate = round(rate);
	fti.type = FnumTableType::Fnumber;
	fti.body = new UINT16[768];
	for (int i = 0; i < 768; i++) {
		double freq = pow(2.0, (double)(i + offset) / 768.0);
		double sig = round(freq * 1024);
		fti.body[i] = UINT16(sig);
	}
	tablelist.push_back(fti);
	return GetFnumTable(master, devide, offset);
}

// Build or get Tone No. table (for SAA1099)
const UINT16* CFnumTable::GetSAATable(int master, int devide, int offset)
{
	double rate = master / devide;
	for (int i = 0; i< tablelist.size(); i++) {
		if (round(rate) == tablelist[i].rate && tablelist[i].type == FnumTableType::saa) {
			return (const UINT16*)tablelist[i].body;
		}
	}
	FnumTableInfo fti;
	fti.rate = round(rate);
	fti.type = FnumTableType::saa;
	fti.body = new UINT16[768];
	for (int i = 0; i < 768; i++) {
		double freq = TuningFrequency * pow(2.0, (double)(i + offset) / 768.0);
		double sig = round(256.0 * log2(freq / rate));
		fti.body[i] = UINT16(sig) & 0xff;
	}
	tablelist.push_back(fti);
	return GetDeltaNTable(master, devide, offset);
}
