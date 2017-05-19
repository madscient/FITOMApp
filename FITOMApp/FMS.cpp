#include "STDAFX.h"
#include "FITOM.h"
#include "FMS.h"
#include "MIDI.h"


CFMS::CFMS(CPort* pt, int fsamp) : CSoundDevice(DEVICE_FMS, 0, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100)
{
	SetReg(0xfe, 0);
	for (int i=0; i<16; i++) {
		SetReg(i, 0);
	}
}

void CFMS::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	SetReg(0xfe, 0);
	SetReg(ch, (GetReg(ch, 0) & 0x80) | evol);
}

void CFMS::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	UINT8 i = 0;
	UINT8 ex = 0;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	SetReg(0xfe, 1);
	for(i=0; i<2; i++)
	{
		UINT8 fb = i ? (voice->FB & 0x70) : (voice->FB << 4);
		SetReg(0x00 + ch + (i<<4), fb | voice->op[i].MUL);
		tmp = (voice->op[i].WS & 3) | (voice->op[i].EGS ? 0x10 : 0) | 0x0c;
		SetReg(0x20 + ch + (i<<4), tmp);
		SetReg(0x40 + ch, 0);	//Pitch LFO (currently disabled)
		SetReg(0x60 + ch, 0);	//Pitch LFO (currently disabled)
		tmp = (voice->op[i].AR << 4) | (voice->op[i].DR & 0xf);
		SetReg(0x80 + ch, tmp);
		tmp = ((voice->op[i].SL & 7) << 4) | (voice->op[i].SR & 0xf);
		SetReg(0xa0 + ch, tmp);
	}
}

void CFMS::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
}

void CFMS::UpdatePanpot(UINT8 ch)
{
}

void CFMS::UpdateSustain(UINT8 ch)
{
}

void CFMS::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
}

void CFMS::UpdateKey(UINT8 ch, UINT8 keyon)
{
}

