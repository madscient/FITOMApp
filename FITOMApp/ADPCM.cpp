#include "STDAFX.H"
#include "ADPCM.h"
#include "MIDI.h"

#define YMDELTA_OFFSET	448
#define YMZ280_OFFSET	356

CAdPcmBase::CAdPcmBase(CPort* pt, int fsamp, int devide, int offset, size_t memsize, UINT8 maxch, UINT8 pardev)
	: CSoundDevice(DEVICE_ADPCM, maxch, fsamp, devide, offset, FnumTableType::DeltaN, pt)
	, maxmem(memsize), usedmem(0), boundary(0), parentdev(pardev), fcode(fsamp)
{
	::ZeroMemory((void*)&adpcmvoice, sizeof(adpcmvoice));
	ops = 0;
	fmaster = fsamp / devide;
	NoteOffset = -57;	// origin note: O3A
}

CYmDelta::CYmDelta(CPort* pt, int fsamp, int devide, size_t memsize, UINT8 pardev, const REGMAP& regset)
	: CAdPcmBase(pt, fsamp, devide, YMDELTA_OFFSET, memsize, 1, pardev), regmap(regset)
{
}

UINT16 CYmDelta::GetDeltaN(int off)
{
	int oct = 0;
	while (off > 1216) {
		off -= 768;
	}
	while (!((YMDELTA_OFFSET - oct * 768) <= off && off <= ((YMDELTA_OFFSET + 768) - oct * 768))) {
		oct++;
	}
	off += 768 * oct;
	UINT ret = Fnum[off - YMDELTA_OFFSET] >> oct;
	return ret;
}

ISoundDevice::FNUM CYmDelta::GetFnumber(UINT8 ch, SINT16 offset)
{
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	int oct = (-NoteOffset / 12) - 1;
	FNUM ret;
	ret.block = oct;
	ret.fnum = GetDeltaN(index);
	return ret;
}

void CYmDelta::LoadVoice(int prog, UINT8* data, size_t length)
{
	UINT32 st = 0;
	UINT32 ed = 0;
	if (prog) {
		st = adpcmvoice[prog - 1].staddr + adpcmvoice[prog - 1].length;
	}
	size_t blk = (length * 8) >> 5;
	if ((blk << 5) < length) { blk++; }
	blk <<= 5;
	if ((st >> 18) != ((st + blk) >> 18)) { //break the 256Kbit boundary
		st = ((st >> 18) + 1) << 18;
	}
	ed = st + blk - 1;
	adpcmvoice[prog].staddr = st;
	adpcmvoice[prog].length = blk;
	SetReg(regmap.flag, 0);
	SetReg(regmap.flag, 0x80);
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0x60);
	SetReg(regmap.control2, regmap.ctrl2init);
	SetReg(regmap.startLSB, (st >> 5) & 0xff);
	SetReg(regmap.startMSB, (st >> 13) & 0xff);
	SetReg(regmap.endLSB, (ed >> 5) & 0xff);
	SetReg(regmap.endMSB, (ed >> 13) & 0xff);
	(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : 0;
	(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : 0;
	for (size_t i = 0; i < (blk >> 3); i++) {
		if (i < length) {
			SetReg(regmap.memory, data[i]);
		}
		else {// padding
			SetReg(regmap.memory, 0);
		}
	}
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0);
}

void CYmDelta::PlayPCM(UINT8 ch, UINT8 num, UINT8 volume, SINT8 pan, SINT32 offset)
{
	if (num < 128 && adpcmvoice[num].length != 0) {
		pan /= 16;
		UINT8 chena = 0;
		if (pan >= 4) { //R
			chena = 0x40;
		}
		else if (pan <= -4) { //L
			chena = 0x80;
		}
		else { //C
			chena = 0xc0;
		}
		UINT32 st = adpcmvoice[num].staddr;
		UINT32 ed = st + adpcmvoice[num].length - 1;
		UINT32 deltan = GetDeltaN(offset);
		SetReg(regmap.flag, 0x1b);
		SetReg(regmap.flag, 0x80);
		SetReg(regmap.control1, 0x20);
		SetReg(regmap.control2, regmap.ctrl2init | (chena & regmap.panmask));
		SetReg(regmap.startLSB, (st >> 5) & 0xff);
		SetReg(regmap.startMSB, (st >> 13) & 0xff);
		SetReg(regmap.endLSB, (ed >> 5) & 0xff);
		SetReg(regmap.endMSB, (ed >> 13) & 0xff);
		(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : 0;
		(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : 0;
		SetReg(regmap.deltanLSB, (deltan)& 0xff);
		SetReg(regmap.deltanMSB, (deltan >> 8) & 0xff);
		SetReg(regmap.volume, (volume << 1) | (volume >> 6));
		SetReg(regmap.control1, 0xa0);
	}
}

void CYmDelta::StopPCM(UINT8 ch, UINT8 num)
{
	SetReg(regmap.control1, GetReg(regmap.control1, 0) & 0x7f);
	SetReg(regmap.control1, 0);
	SetReg(regmap.control1, regmap.ctrl1init);
	SetReg(regmap.control1, 0);
}

void CYmDelta::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	int offset = fnum ? (SINT16)fnum->fnum : 0;
	StopPCM(0, num);
	PlayPCM(0, num, CalcVolExpVel(rhythmvol, 127, vel), pan, offset);
}

void CYmDelta::RhythmOff(UINT8 num)
{
	StopPCM(0, num);
}


void CYmDelta::UpdateKey(UINT8 ch, UINT8 keyon)
{
	if (ch < chs) {
		StopPCM(ch, 0);
		if (keyon) {
			SetReg(regmap.flag, 0x1b);
			SetReg(regmap.flag, 0x80);
			SetReg(regmap.control1, 0xa0);
		}
	}
}

void CYmDelta::UpdateVolExp(UINT8 ch)
{
	UINT8 volume = GetChAttribute(ch)->GetEffectiveLevel();
	SetReg(regmap.volume, (volume << 1) | (volume >> 6));
}

void CYmDelta::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(regmap.deltanLSB, (fnum->fnum)& 0xff);
	SetReg(regmap.deltanMSB, (fnum->fnum >> 8) & 0xff);
}

void CYmDelta::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	UINT8 chena = 0;
	if (pan >= 4) { //R
		chena = 0x40;
	}
	else if (pan <= -4) { //L
		chena = 0x80;
	}
	else { //C
		chena = 0xc0;
	}
	SetReg(regmap.control2, regmap.ctrl2init | (chena & regmap.panmask));
}

void CYmDelta::UpdateVoice(UINT8 ch)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	int num = voice->AL;
	UINT32 st = adpcmvoice[num].staddr;
	UINT32 ed = st + adpcmvoice[num].length - 1;
	SetReg(regmap.startLSB, (st >> 5) & 0xff);
	SetReg(regmap.startMSB, (st >> 13) & 0xff);
	SetReg(regmap.endLSB, (ed >> 5) & 0xff);
	SetReg(regmap.endMSB, (ed >> 13) & 0xff);
	(regmap.limitLSB != 0xff) ? SetReg(regmap.limitLSB, (ed >> 5) & 0xff) : 0;
	(regmap.limitMSB != 0xff) ? SetReg(regmap.limitMSB, (ed >> 13) & 0xff) : 0;
}

//CAdPcm3801 YM3801 aka Y8950
CAdPcm3801::CAdPcm3801(CPort* pt, int fsamp, size_t memsize) : CYmDelta(pt, fsamp, 72, memsize, DEVICE_Y8950, 
{ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0xff, 0xff, 0x0f, 0x10, 0x11, 0x12, 0x04, 0x01, 0x00, 0x00 })
{
	boundary = 32;
	SetReg(0x06, 0);
	SetReg(0x07, 0);
}

//CAdPcm2608 YM2608 aka OPNA
CAdPcm2608::CAdPcm2608(CPort* pt, int fsamp, size_t memsize) : CYmDelta(pt, fsamp, 144, memsize, DEVICE_OPNA,
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x01, 0x00, 0xc0 })
{
	boundary = 32;
	SetReg(0x00, 0);
	SetReg(0x01, 2);
}

//CAdPcmZ280 YMZ280 aka PCMD8
CAdPcmZ280::CAdPcmZ280(CPort* pt, int fsamp, UINT32 memsize)
	: CAdPcmBase(pt, fsamp, 384, YMZ280_OFFSET, memsize, 8, DEVICE_PCMD8)
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

