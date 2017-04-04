#include "stdafx.h"
#include "MasterVolumeCtrl.h"


CMasterVolumeCtrl::CMasterVolumeCtrl(const char* param) : volume(0)
{
}


CMasterVolumeCtrl::~CMasterVolumeCtrl()
{
}

void CMasterVolumeCtrl::SetVolume(uint32_t vol)
{
	volume = vol;
	UpdateVolume();
}

uint32_t CMasterVolumeCtrl::GetVolume()
{
	return volume;
}

void CMasterVolumeCtrl::UpdateVolume()
{
}