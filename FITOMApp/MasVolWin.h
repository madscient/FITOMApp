#pragma once
#include "MasterVolumeCtrl.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class CMasVolWin32 : public CMasterVolumeCtrl
{
protected:
	IAudioEndpointVolume* pVol;
	float volmin, volmax, volstep;
	virtual void UpdateVolume();
public:
	CMasVolWin32(const TCHAR* param);
	~CMasVolWin32();
};
