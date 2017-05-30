#include "STDAFX.H"
#include "ADPCM.h"
#include "MIDI.h"

CAdPcmBase::CAdPcmBase(UINT8 devid, CPort* pt, size_t regsize, int fsamp, int devide, int offset, size_t memsize, UINT8 maxch, UINT8 pardev)
	: CSoundDevice(devid, maxch, fsamp, devide, offset, FnumTableType::DeltaN, pt, regsize)
	, maxmem(memsize), usedmem(0), boundary(0), parentdev(pardev), fcode(fsamp)
{
	::ZeroMemory((void*)&adpcmvoice, sizeof(adpcmvoice));
	ops = 0;
	fmaster = devide ? (fsamp / devide) : 0;
	NoteOffset = -57;	// origin note: O3A
}

void CAdPcmBase::Debug()
{
#ifdef _DEBUG
	fprintf(stderr, _T("%08X:%s\n"), CFITOM::GetInstance()->GetDeviceUniqID(this), CFITOM::GetInstance()->GetDeviceNameFromID(GetDevice()));
	for (int i = 0; i < 128; i++) {
		fprintf(stderr, _T("%i:%s 0x%08X,0x%08X\n"), i, adpcmvoice[i].name, adpcmvoice[i].staddr, adpcmvoice[i].length);
	}
#endif

}
