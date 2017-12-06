#include "STDAFX.H"
#include "MultiDev.h"
#define _USE_MATH_DEFINES // for C++  
#include <math.h>  

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
	int pan = (attr->panpot - 1);
	pan = (pan < 0) ? 0 : pan;
	double lgain = cos(M_PI_2 * pan / 126.0);
	double rgain = sin(M_PI_2 * pan / 126.0);
	chips[0]->SetVolume(ch, round(lgain * double(attr->volume)), 1);
	chips[1]->SetVolume(ch, round(rgain * double(attr->volume)), 1);
}

void CLinearPan::SetVolume(UINT8 ch, UINT8 vol, int update)
{
	CSoundDevice::SetVolume(ch, vol, update);
}

void CLinearPan::UpdateVolExp(UINT8 ch)
{
	UpdatePanpot(ch);
}
