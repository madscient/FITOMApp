#include "STDAFX.H"
#include "MultiDev.h"

CSpanDevice::CSpanDevice(CSoundDevice* chip1, CSoundDevice* chip2)
	: CMultiDevice(NULL, NULL), chres(NULL), prior_dev(0)
{
	if (chip1 && chip2) {
		AddDevice(chip1);
		AddDevice(chip2);
		SetDevice(chip1->GetDevice());
	}
	delete[] chattr;
	chattr = 0;
}

void CSpanDevice::AddDevice(CSoundDevice* chip)
{
	UINT8 newchs = chs + chip->GetAvailable();
	int dev = numchips;
	CMultiDevice::AddDevice(chip);

	DEVCHRES* newchres = new DEVCHRES[newchs];
	if (chres) {
		for (int i=0; i<chs; i++) {
			newchres[i] = chres[i];
		}
		delete [] chres;
	}
	chres = newchres;

	/*
	CHATTR* newchattr = new CHATTR[newchs];
	if (chattr) {
		for (int i=0; i<chs; i++) {
			newchattr[i] = chattr[i];
		}
		delete [] chattr;
	}
	chattr = newchattr;
	*/

	for (int dch=0; dch<chip->GetChs(); dch++) {
		if (chip->GetChAttribute(dch)->IsEnable()) {
			chres[chs].dev = dev;
			chres[chs].ch = dch;
			chs++;
		}
	}
}

ISoundDevice::CHATTR* CSpanDevice::GetChAttribute(UINT8 ch) const
{
	return (ch < chs) ? chips[chres[ch].dev]->GetChAttribute(chres[ch].ch) : 0;
}

void CSpanDevice::UpdateVoice(UINT8 ch)
{
	chips[chres[ch].dev]->UpdateVoice(chres[ch].ch);
}

void CSpanDevice::UpdateVolExp(UINT8 ch)
{
	chips[chres[ch].dev]->UpdateVolExp(chres[ch].ch);
}

void CSpanDevice::UpdatePanpot(UINT8 ch)
{
	chips[chres[ch].dev]->UpdatePanpot(chres[ch].ch);
}

void CSpanDevice::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	chips[chres[ch].dev]->UpdateFreq(chres[ch].ch, fnum);
}

void CSpanDevice::UpdateSustain(UINT8 ch)
{
	chips[chres[ch].dev]->UpdateSustain(chres[ch].ch);
}

void CSpanDevice::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	chips[chres[ch].dev]->UpdateTL(chres[ch].ch, op, lev);
}

void CSpanDevice::UpdateKey(UINT8 ch, UINT8 keyon)
{
	chips[chres[ch].dev]->UpdateKey(chres[ch].ch, keyon);
}

void CSpanDevice::UpdateFnumber(UINT8 ch, int update)
{
	chips[chres[ch].dev]->UpdateFnumber(chres[ch].ch, update);
}

UINT8 CSpanDevice::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	UINT8 dev = 0xff;
	UINT8 ch = 0xff;

	for (int i=0; i<numchips && ch==0xff; i++) {
		dev = (prior_dev + i) % numchips;
		ch = chips[dev]->QueryCh(parent, voice, mode);
	}
	if (ch != 0xff) {
		for (int j=0; j<chs; j++) {
			if (chres[j].dev == dev && chres[j].ch == ch) {
				ret = j;
				break;
			}
		}
	}
	if (ret != 0xff) {
		prior_dev = dev;
	}
	return ret;
}

void CSpanDevice::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	UINT8 cap = chips[0]->GetRhythmCaps();
	if (cap) {
		UINT8 dev = num / cap;
		if (dev < numchips) {
			chips[dev]->RhythmOn(num % cap, vel, pan, rv, fnum);
		}
	}
}

int CSpanDevice::LowLevelCommand(UINT8 ch, PERFCMD cmd)
{
	return chips[chres[ch].dev]->LowLevelCommand(chres[ch].ch, cmd);
}
