//----------------------------------------------------------------------
//	Sound Chip common Interface
//----------------------------------------------------------------------
#pragma once
#include	<Windows.h>

// Sound Interface Infomation
typedef struct {
	char	cInterfaceName[64];			// Interface Name
	int		iSoundChipCount;			// Sound Chip Count;
} SCCI_INTERFACE_INFO;

// Sound Chip Infomation
typedef struct {
	char	cSoundChipName[64];			// Sound Chip Name
	int		iSoundChip;					// Sound Chip ID
	int		iCompatibleSoundChip[2];	// Compatible Sound Chip ID
	DWORD	dClock;						// Sound Chip clock
	DWORD	dCompatibleClock[2];		// Sound Chip clock
	bool	bIsUsed;					// Sound Chip Used Check
	DWORD	dBusID;						// 接続バスID
	DWORD	dSoundLocation;				// サウンドロケーション
} SCCI_SOUND_CHIP_INFO;

class	SoundInterfaceManager;
class	SoundInterface;
class	SoundChip;

//----------------------------------------
// Sound Interface Manager
//----------------------------------------
class	SoundInterfaceManager{
public:
	// ---------- LOW LEVEL APIs ----------
	// get interface count
	virtual int __stdcall getInterfaceCount() = 0;
	// get interface information 
	virtual SCCI_INTERFACE_INFO* __stdcall getInterfaceInfo(int iInterfaceNo) = 0;
	// get interface instance
	virtual SoundInterface* __stdcall getInterface(int iInterfaceNo) = 0;
	// release interface instance
	virtual bool __stdcall releaseInterface(SoundInterface* pSoundInterface) = 0;
	// release all interface instance
	virtual bool __stdcall releaseAllInterface() = 0;
	// ---------- HI LEVEL APIs ----------
	// get sound chip instance
	virtual SoundChip* __stdcall getSoundChip(int iSoundChipType,DWORD dClock) = 0;
	// release sound chip instance
	virtual bool __stdcall releaseSoundChip(SoundChip* pSoundChip) = 0;
	// release all sound chip instance
	virtual bool __stdcall releaseAllSoundChip() = 0;
	// set delay time
	virtual bool __stdcall setDelay(DWORD dMSec) = 0;
	// get delay time
	virtual DWORD __stdcall getDelay() = 0;
	// reset interfaces(A sound chips initialize after interface reset)
	virtual bool __stdcall reset() = 0;
	// initialize sound chips
	virtual bool __stdcall init() = 0;
	// Sound Interface instance initialize
	virtual	bool __stdcall initializeInstance() = 0;
	// Sound Interface instance release
	virtual bool __stdcall releaseInstance() = 0;
	// config scci
	// !!!this function is scciconfig exclusive use!!!
	virtual bool __stdcall config() = 0;
	// get version info
	virtual DWORD __stdcall getVersion(DWORD *pMVersion = NULL) = 0;
	// get Level mater disp valid
	virtual bool __stdcall isValidLevelDisp() = 0;
	// get Level mater disp visible
	virtual bool __stdcall isLevelDisp() = 0;
	// set Level mater disp visible
	virtual void __stdcall setLevelDisp(bool bDisp) = 0;
	// set mode
	virtual void __stdcall setMode(int iMode) = 0;
	// send datas
	virtual void __stdcall sendData() = 0;
	// clear buffer
	virtual void __stdcall clearBuff() = 0;
	// set Acquisition Mode(Sound Chip)
	virtual void __stdcall setAcquisitionMode(int iMode) = 0;
	// set Acquisition Mode clock renge
	virtual void __stdcall setAcquisitionModeClockRenge(DWORD dClock) = 0;
	// set command buffer size
	virtual bool __stdcall setCommandBuffetSize(DWORD dBuffSize) = 0;
	// buffer check
	virtual bool __stdcall isBufferEmpty() = 0;
};

//----------------------------------------
// Sound Interface(LOW level APIs)
//----------------------------------------
class	SoundInterface{
public:
	// support low level API check
	virtual bool __stdcall isSupportLowLevelApi() = 0;
	// send data to interface
	virtual bool __stdcall setData(BYTE *pData,DWORD dSendDataLen) = 0;
	// get data from interface
	virtual DWORD __stdcall getData(BYTE *pData,DWORD dGetDataLen) = 0;
	// set delay time
	virtual	bool __stdcall setDelay(DWORD dDelay) = 0;
	// get delay time
	virtual DWORD __stdcall getDelay() = 0;
	// reset interface
	virtual bool __stdcall reset() = 0;
	// initialize sound chips
	virtual bool __stdcall init() = 0;
	// サウンドチップ数取得
	virtual DWORD	__stdcall getSoundChipCount() = 0;
	// サウンドチップ取得
	virtual	SoundChip* __stdcall getSoundChip(DWORD dNum) = 0;
};

//----------------------------------------
// Sound Chip
//----------------------------------------
class	SoundChip{
public:
	// get sound chip information
	virtual SCCI_SOUND_CHIP_INFO* __stdcall getSoundChipInfo() = 0;
	// get sound chip type
	virtual int __stdcall getSoundChipType() = 0;
	// set Register data
	virtual bool __stdcall setRegister(DWORD dAddr,DWORD dData) = 0;
	// get Register data(It may not be supported)
	virtual DWORD __stdcall getRegister(DWORD dAddr) = 0;
	// initialize sound chip(clear registers)
	virtual bool __stdcall init() = 0;
	// get sound chip clock
	virtual DWORD __stdcall getSoundChipClock() = 0;
	// get writed register data
	virtual DWORD __stdcall getWrittenRegisterData(DWORD addr) = 0;
	// buffer check
	virtual bool __stdcall isBufferEmpty() = 0;
};

//----------------------------------------
// get sound interface manager function
//----------------------------------------
typedef SoundInterfaceManager* (__stdcall *SCCIFUNC)(void);

