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
	uint8_t newchs = chs + chip->GetAvailable();
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

ISoundDevice::CHATTR* CSpanDevice::GetChAttribute(uint8_t ch) const
{
	return (ch < chs) ? chips[chres[ch].dev]->GetChAttribute(chres[ch].ch) : 0;
}

void CSpanDevice::UpdateVoice(uint8_t ch)
{
	chips[chres[ch].dev]->UpdateVoice(chres[ch].ch);
}

void CSpanDevice::UpdateVolExp(uint8_t ch)
{
	chips[chres[ch].dev]->UpdateVolExp(chres[ch].ch);
}

void CSpanDevice::UpdatePanpot(uint8_t ch)
{
	chips[chres[ch].dev]->UpdatePanpot(chres[ch].ch);
}

void CSpanDevice::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	chips[chres[ch].dev]->UpdateFreq(chres[ch].ch, fnum);
}

void CSpanDevice::UpdateSustain(uint8_t ch)
{
	chips[chres[ch].dev]->UpdateSustain(chres[ch].ch);
}

void CSpanDevice::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	chips[chres[ch].dev]->UpdateTL(chres[ch].ch, op, lev);
}

void CSpanDevice::UpdateKey(uint8_t ch, uint8_t keyon)
{
	chips[chres[ch].dev]->UpdateKey(chres[ch].ch, keyon);
}

void CSpanDevice::UpdateFnumber(uint8_t ch, int update)
{
	chips[chres[ch].dev]->UpdateFnumber(chres[ch].ch, update);
}

uint8_t CSpanDevice::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	uint8_t ret = 0xff;
	volatile uint8_t dev = 0xff;
	volatile uint8_t ch = 0xff;

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
		dev = (prior_dev + 1) % numchips;
	}
	return ret;
}
