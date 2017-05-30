#include "stdafx.h"
#include "PCMD8.h"

#define YMZ280_OFFSET	356

//CAdPcmZ280 YMZ280 aka PCMD8
CAdPcmZ280::CAdPcmZ280(CPort* pt, int fsamp, size_t memsize)
	: CAdPcmBase(DEVICE_ADPCM, pt, 0x100, fsamp, 384, YMZ280_OFFSET, memsize, 8, DEVICE_PCMD8)
{
	SetReg(0xff, 0xc0);	// KON enable/Memory enable
	SetReg(0x81, 0);	// DSP disable
	for (int i = 0; i < 8; i++) {
		SetReg(0x80, 0x88 | (i * 17));
	}
}

UINT16 CAdPcmZ280::GetDeltaN(int off)
{
	int oct = 0;
	while (off > 1216) {
		off -= 768;
	}
	while (!((YMZ280_OFFSET - oct * 768) <= off && off <= ((YMZ280_OFFSET + 768) - oct * 768))) {
		oct++;
	}
	off += 768 * oct;
	UINT ret = Fnum[off - YMZ280_OFFSET] >> oct;
	return ret;
}

ISoundDevice::FNUM CAdPcmZ280::GetFnumber(UINT8 ch, SINT16 offset)
{
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	FNUM ret;
	ret.block = 0;
	ret.fnum = GetDeltaN(index);
	return ret;
}

void CAdPcmZ280::LoadVoice(int prog, UINT8* data, size_t length)
{
	UINT32 st = 0;
	UINT32 ed = 0;
	if (prog) {
		st = adpcmvoice[prog - 1].staddr + adpcmvoice[prog - 1].length;
	}
	size_t blk = length >> 2;
	if ((blk << 2) < length) { blk++; }
	blk <<= 2;
	ed = st + blk - 1;
	adpcmvoice[prog].staddr = st;
	adpcmvoice[prog].length = blk;
	for (size_t i = 0; i < blk; i++) {
		UINT32 addr = st + i;
		SetReg(0x84, addr >> 16);
		SetReg(0x85, (addr >> 8) & 0xff);
		SetReg(0x86, addr & 0xff);
		SetReg(0x87, (i < length) ? data[i] : 0x0);
	}
}

void CAdPcmZ280::UpdateKey(UINT8 ch, UINT8 keyon)
{
	UINT8 tmp = GetReg(0x01 + ch * 4, 0) & 1;
	SetReg(0x01 + ch * 4, (keyon ? 0xa0 : 0x20) | tmp);
}

void CAdPcmZ280::UpdateVolExp(UINT8 ch)
{
	UINT8 volume = GetChAttribute(ch)->GetEffectiveLevel();
	SetReg(0x02 + ch * 4, (volume << 1) | (volume >> 6));
}

void CAdPcmZ280::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x00 + ch * 4, (fnum->fnum >> 8) & 0xff);
	UINT8 tmp = (GetReg(0x01 + ch * 4, 0) & 0xfe);
	SetReg(0x01 + ch * 4, tmp | ((fnum->fnum >> 16) & 1));
}

void CAdPcmZ280::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	SetReg(0x3 + ch * 4, pan & 0xf);
}

void CAdPcmZ280::UpdateVoice(UINT8 ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL;
	UINT32 st = adpcmvoice[num].staddr;
	UINT32 ed = st + adpcmvoice[num].length - 1;
	SetReg(0x20 + ch * 4, (st >> 16) & 0xff);
	SetReg(0x40 + ch * 4, (st >> 8) & 0xff);
	SetReg(0x60 + ch * 4, st & 0xff);
	SetReg(0x23 + ch * 4, (ed >> 16) & 0xff);
	SetReg(0x43 + ch * 4, (ed >> 8) & 0xff);
	SetReg(0x63 + ch * 4, ed & 0xff);

	SetReg(0x21 + ch * 4, 0);
	SetReg(0x41 + ch * 4, 0);
	SetReg(0x61 + ch * 4, 0);
	SetReg(0x22 + ch * 4, 0);
	SetReg(0x42 + ch * 4, 0);
	SetReg(0x62 + ch * 4, 0);
}

