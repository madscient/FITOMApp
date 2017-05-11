#include "stdafx.h"
#include "SoundDev.h"
#include "OPL4.h"
#include "MIDI.h"

//COPL4ML YMF704 aka OPL4-ML
COPL4ML::COPL4ML(CPort* pt, int fsamp) :
CSoundDevice(DEVICE_OPL4ML, 24, fsamp, 684, 0, FnumTableType::none, pt, 0x100)
{
	Fnum = theFnum.GetOPL4Table(fsamp, 684, 0);
	NoteOffset = 0;	//Origin = O3A
	SetReg(0x02, 0x40);
	SetReg(0xf8, 0x3f);
	SetReg(0xf9, 0x3f);
}

ISoundDevice::FNUM COPL4ML::GetFnumber(UINT8 ch, SINT16 offset)
{
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	FNUM ret;
	int oct = (index / 768) - 2;
	index = index % 768;
	ret.fnum = Fnum[index];
	ret.block = UINT8(oct);
	return ret;
}

void COPL4ML::UpdateKey(UINT8 ch, UINT8 keyon)
{
	UINT8 tmp = GetReg(0x68 + ch, 0) & 0x7f;
	SetReg(0x68 + ch, tmp | (keyon ? 0x80 : 0));
	CHATTR* attr = GetChAttribute(ch);
	if (attr->GetParent()) {
		tmp = GetReg(0x38 + ch, 0) & 0xf7;
		SetReg(0x38 + ch, tmp | (attr->GetParent()->GetSustain() ? 0x08 : 0));
	}
}

void COPL4ML::UpdateVolExp(UINT8 ch)
{
	UINT8 volume = GetChAttribute(ch)->GetEffectiveLevel();
	SetReg(0x50 + ch, (volume << 1) | 1);
}

void COPL4ML::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	UINT8 tmp = GetReg(0x38 + ch, 1) & 0x08;
	SetReg(0x38 + ch, tmp | (fnum->block << 4) | (fnum->fnum >> 7));
	SetReg(0x20 + ch, (fnum->fnum << 1) & 0xff);
}

void COPL4ML::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	UINT8 tmp = GetReg(0x68 + ch, 0) & 0xf0;
	SetReg(0x68 + ch, tmp | (UINT8(pan) & 0xf));
}

void COPL4ML::UpdateVoice(UINT8 ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL | ((int)voice->FB << 7);
	UINT8 tmp = GetReg(0x20 + ch, 0) & 0xfe;
	SetReg(0x20 + ch, (num >> 8) & 1);
	SetReg(0x08 + ch, (num & 0xff));
}

COPL4ML2::COPL4ML2(CPort* pt, int fsamp) : COPL4ML(pt, fsamp)
{
	SetDevice(DEVICE_OPL4ML2);
}
