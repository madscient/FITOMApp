#include "stdafx.h"
#include "DCSG.h"


//-------------------------------
CDCSG::CDCSG(CPort* pt, int fsamp)
	: CPSGBase(DEVICE_DCSG, pt, 0x10, 4, fsamp, 2, -576, FnumTableType::TonePeriod)
	, prevnoise(0)
{
	port->write(0, 0x80);
	port->write(0, 0x00);
	port->write(0, 0x9f);
	port->write(0, 0xa0);
	port->write(0, 0x00);
	port->write(0, 0xbf);
	port->write(0, 0xc0);
	port->write(0, 0x00);
	port->write(0, 0xdf);
	port->write(0, 0xe0);
	port->write(0, 0xff);
	GetChAttribute(3)->OutOfDVA();	// Noise ch should always be assigned manually
	for (int i = 0; i < 4; i++) {
		prevvol[i] = 0;
		prevfreq[i] = 0;
	}
}

void CDCSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	evol = Linear2dB(CalcLinearLevel(evol, 127 - egattr[ch].GetValue()), RANGE48DB, STEP300DB, 4);
	if (prevvol[ch] != evol) {
		prevvol[ch] = evol;
		if (ch < 3) {
			port->writeRaw(0, 0x90 | (ch * 32) | (evol & 0xf));
		}
		else {
			port->writeRaw(0, 0xf0 | (evol & 0xf));
		}
	}
}

void CDCSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	if (ch < 3) {
		UINT8 oct = fnum->block;
		UINT16 etp = fnum->fnum >> (oct + 3);
		if (etp != prevfreq[ch]) {
			prevfreq[ch] = etp;
			port->writeRaw(0, 0x80 | (ch * 32) | (etp & 0xf));
			port->writeRaw(0, 0x0 | ((etp >> 4) & 63));
		}
	}
	else {
	}
}

void CDCSG::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	switch (ch) {
	case 0:
	case 1:
	case 2:
		break;
	case 3:
		if (attr->GetVoice()->AL == 1) {
			prevnoise = 0xe0 | ((attr->GetVoice()->FB & 1) << 2) | (attr->GetVoice()->NFQ & 0x3);
			port->writeRaw(0, prevnoise);
		}
		break;
	}
}

UINT8 CDCSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	if (voice && voice->AL == 1) {
		ret = 3;
	}
	else {
		ret = CSoundDevice::QueryCh(parent, voice, mode);
	}
	return ret;
}

