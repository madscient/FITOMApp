#pragma once
#include "MasterVolumeCtrl.h"
class CMasterVolumeWin :
	public CMasterVolumeCtrl
{
protected:
	virtual void UpdateVolume();
public:
	CMasterVolumeWin(const char* param);
	~CMasterVolumeWin();
};

