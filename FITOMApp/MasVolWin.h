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
	CMasVolWin32(const char* param);
	~CMasVolWin32();
};

class CMasVolWinXp : public CMasterVolumeCtrl
{
protected:
	HANDLE hVol;
	virtual void UpdateVolume();
public:
	CMasVolWinXp() {};
	CMasVolWinXp(const char* param);
	~CMasVolWinXp();
};