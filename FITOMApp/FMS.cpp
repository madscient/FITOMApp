#include "STDAFX.h"
#include "FITOM.h"
#include "FMS.h"
#include "MIDI.h"


CFMS::CFMS(CPort* pt, int fsamp) : CSoundDevice(DEVICE_FMS, 16, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100), prevkey(0)
{
	for (int i=0; i<16; i++) {
		prevvol[i] = 0;
	}
}

void CFMS::Init()
{
	SetReg(0xfe, 0);
	for (int i = 0; i<16; i++) {
		SetReg(i, 0);
	}
}

void CFMS::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	uint8_t evol = Linear2dB(attr->GetEffectiveLevel(), RANGE96DB, STEP075DB, 7);
	SetReg(0xfe, 0);
	SetReg(ch, ((prevkey & (1 << ch)) ? 0x80 : 0) | evol);
	prevvol[ch] = evol;
}

void CFMS::UpdateVoice(uint8_t ch)
{
	uint8_t tmp;
	uint8_t i = 0;
	uint8_t ex = 0;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	SetReg(0xfe, 1);	//Bank select "1"
	for(i=0; i<2; i++)
	{
		uint8_t fb = i ? (voice->FB & 0x70) : (voice->FB << 4);
		SetReg(0x00 + ch + (i<<4), fb | voice->op[i].MUL);
		tmp = (voice->op[i].WS & 3) | (voice->op[i].SR ? 0x10 : 0) | 0x0c;
		SetReg(0x20 + ch + (i<<4), tmp);
		SetReg(0x40 + ch, 0);	//Pitch LFO (currently disabled)
		SetReg(0x60 + ch, 0);	//Pitch LFO (currently disabled)
		tmp = (voice->op[i].AR << 4) | (voice->op[i].DR & 0xf);
		SetReg(0x80 + ch, tmp);
		tmp = ((voice->op[i].SL & 7) << 4) | (voice->op[i].SR & 0xf);
		SetReg(0xa0 + ch, tmp);
	}
}

void CFMS::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	SetReg(0xfe, 0);	//Bank select "0"

}

void CFMS::UpdatePanpot(uint8_t ch)
{
	SetReg(0xfe, 0);	//Bank select "0"
}

void CFMS::UpdateSustain(uint8_t ch)
{
	SetReg(0xfe, 1);	//Bank select "1"
}

void CFMS::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
}

void CFMS::UpdateKey(uint8_t ch, uint8_t keyon)
{
	SetReg(0xfe, 0);	//Bank select "0"
	SetReg(0x00 + ch, (keyon ? 0x80 : 0) | prevvol[ch], 1);
	prevkey = keyon ? (prevkey | (1 << ch)) : (prevkey & ~(1 << ch));
}

