#include "STDAFX.H"
#include "MultiDev.h"

CSplitPan::CSplitPan(CSoundDevice* chip1, CSoundDevice* chip2) : CUnison(chip1, chip2)
{
}

void CSplitPan::UpdateKey(UINT8 ch, UINT8 keyon)
{
	int pan = (GetChAttribute(ch)->panpot - 64) / 16;
	if (pan == 0 || !keyon) { //C
		CUnison::UpdateKey(ch, keyon);
	} else if (pan > 0) { //R
		chips[1]->UpdateKey(ch, keyon);
	} else if (pan < 0) { //L
		chips[0]->UpdateKey(ch, keyon);
	}
}

void CSplitPan::UpdatePanpot(UINT8 ch)
{
}

CLinearPan::CLinearPan(CSoundDevice* chip1, CSoundDevice* chip2) : CUnison(chip1, chip2)
{
}

void CLinearPan::UpdatePanpot(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	int pan = (attr->panpot - 64);
	int lvol, rvol;
	if (pan > 0) { // R
		lvol = 63 - pan;
		rvol = 63;
	} else if (pan < 0) {
		lvol = 63;
		rvol = 63 + pan;
	} else {
		lvol = 63;
		rvol = 63;
	}
	lvol = (lvol * attr->volume + lvol + attr->volume) >> 6;
	rvol = (rvol * attr->volume + rvol + attr->volume) >> 6;
	chips[0]->SetVolume(ch, lvol, 1);
	chips[1]->SetVolume(ch, rvol, 1);
}

void CLinearPan::SetVolume(UINT8 ch, UINT8 vol, int update)
{
	CSoundDevice::SetVolume(ch, vol, update);
}

void CLinearPan::UpdateVolExp(UINT8 ch)
{
	UpdatePanpot(ch);
}
