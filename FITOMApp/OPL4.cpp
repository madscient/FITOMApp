#include "stdafx.h"
#include "OPL4.h"

//COPL4ML YMF704 aka OPL4-ML
COPL4ML::COPL4ML(CPort* pt, int fsamp) :
CSoundDevice(DEVICE_OPL4ML, 24, fsamp, 684, 0, FnumTableType::opl4, pt, 0x100)
{
	//Fnum = theFnum.GetOPL4Table(fsamp, 684, 0);
	NoteOffset = 0;	//Origin = O3A
}

void COPL4ML::Init()
{
	SetReg(0x02, 0x40);
	SetReg(0xf8, 0x3f);
	SetReg(0xf9, 0x3f);
}

ISoundDevice::FNUM COPL4ML::GetFnumber(uint8_t ch, int16_t offset)
{
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	FNUM ret;
	int oct = (index / 768) - 2;
	index = index % 768;
	ret.fnum = Fnum[index];
	ret.block = uint8_t(oct);
	return ret;
}

void COPL4ML::UpdateKey(uint8_t ch, uint8_t keyon)
{
	uint8_t tmp = GetReg(0x68 + ch, 0) & 0x7f;
	SetReg(0x68 + ch, tmp | (keyon ? 0x80 : 0));
	CHATTR* attr = GetChAttribute(ch);
	if (attr->GetParent()) {
		tmp = GetReg(0x38 + ch, 0) & 0xf7;
		SetReg(0x38 + ch, tmp | (attr->GetParent()->GetSustain() ? 0x08 : 0));
	}
}

void COPL4ML::UpdateVolExp(uint8_t ch)
{
	uint8_t volume = GetChAttribute(ch)->GetEffectiveLevel();
	SetReg(0x50 + ch, (volume << 1) | 1, 0);
}

void COPL4ML::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	uint8_t tmp = GetReg(0x38 + ch, 1) & 0x08;
	SetReg(0x38 + ch, tmp | (fnum->block << 4) | (fnum->fnum >> 7), 0);
	SetReg(0x20 + ch, (fnum->fnum << 1) & 0xff, 0);
}

void COPL4ML::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t tmp = GetReg(0x68 + ch, 0) & 0xf0;
	SetReg(0x68 + ch, tmp | (uint8_t(pan) & 0xf));
}

void COPL4ML::UpdateVoice(uint8_t ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL | ((int)voice->FB << 7);
	uint8_t tmp = GetReg(0x20 + ch, 0) & 0xfe;
	SetReg(0x20 + ch, (num >> 8) & 1, 0);
	SetReg(0x08 + ch, (num & 0xff), 0);
}

COPL4ML2::COPL4ML2(CPort* pt, int fsamp) : COPL4ML(pt, fsamp)
{
	SetDevice(DEVICE_OPL4ML2);
}
