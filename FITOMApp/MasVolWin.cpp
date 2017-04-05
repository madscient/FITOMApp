#include "stdafx.h"
#include "MasVolWin.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

CMasVolWin32::CMasVolWin32(const char* param) : pVol(0)
{
	TCHAR lineindev[256];
	IMMDeviceEnumerator* pmmenum = 0;
	HRESULT hres = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pmmenum));
	if (SUCCEEDED(hres)) {
		IMMDevice* pDev = 0;
		hres = pmmenum->GetDevice(CT2W(param), &pDev);
		if (SUCCEEDED(hres)) {
			if (SUCCEEDED(pDev->Activate(IID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, nullptr, (void**)&pVol))) {
				valid = TRUE;
			}
		}
		SAFE_RELEASE(pDev);
	}
	SAFE_RELEASE(pmmenum);
}


CMasVolWin32::~CMasVolWin32()
{
	SAFE_RELEASE(pVol);
}

void CMasVolWin32::UpdateVolume()
{
	pVol->SetMasterVolumeLevelScalar(volume / 127.0, NULL);
}

CMasVolWinXp::CMasVolWinXp(const char* param) : hVol(0)
{
}