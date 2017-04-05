#include "stdafx.h"
#include "MasterVolumeCtrl.h"


CMasterVolumeCtrl::CMasterVolumeCtrl() : volume(127), valid(FALSE)
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