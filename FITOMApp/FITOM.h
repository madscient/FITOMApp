#ifndef __FMIDI98_H__
#define __FMIDI98_H__

#define MAX_BANK 8
#define MAX_MPUS 4
#define MAX_DEVS 64
#define MAX_IOIF 16

#define ERROR_MIDI_IN	-1
#define ERROR_FM_DEV	-2
#define ERROR_FM_BANK	-3
#define ERROR_DRUM_MAP	-4
#define ERROR_SCCI		-9

class CSoundDevice;
class CMidiInst;
class CMidiIn;
class CMidiOut;
class IScreen;
class CSCCIWrapper;

#include "Port.h"
#include "FITOMCfg.h"

struct DeviceControlSlice {
	CPort* port;
	UINT16 addr;
	UINT16 data;
};



class CFITOM {
public:
protected:
	CMidiInst* midi[MAX_MPUS];
	CFITOMConfig* theConfig;
	volatile int timerprocessing;
	volatile int pollprocessing;
	TCHAR profile[MAX_PATH];
	BYTE LCDdot[11][16][16];
	BYTE LCDstr[17];
	int LCDdisp;

	//Configuration
	int ImportConfig(CFITOMConfig* config);
	UINT32 MIDItick;
#ifdef _DEBUG
	DWORD g_devno;
#endif

public:
	//Initialize and Finalize
	static CFITOM* GetInstance() {
		static CFITOM theInstance;
		return & theInstance;
	};
	int InitInstance(CFITOMConfig* config);
	void ExitInstance(int save=0);

	//Query
	int GetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 num);
	int GetDrum(DRUMMAP* drum, UINT8 bank, UINT8 prog, UINT8 note);
	CMidiInst* GetMidiInst(UINT8 i) const { return (i<theConfig->GetMidiInputs()) ? midi[i] : NULL; };
	CFMBank* GetFMBank(UINT32 voiceid, UINT32 bank);
	CDrumBank* GetDrumBank(UINT8 prog);
	CPcmBank* GetPcmBank(UINT8 bank);

	const UINT8 GetLogDevs() const { return theConfig ? theConfig->GetLogDevs() : 0; };
	const UINT8 GetPhyDevs() const { return theConfig ? theConfig->GetPhyDevs() : 0; };
	const UINT8 GetPcmDevs() const { return theConfig ? theConfig->GetPcmDevs() : 0; };
	CSoundDevice* GetLogicalDeviceFromID(UINT8 devid) const { return theConfig ? theConfig->GetLogDeviceFromID(devid) : 0; };
	CSoundDevice* GetLogicalDeviceFromIndex(UINT8 i) const { return theConfig ? theConfig->GetLogDeviceFromIndex(i) : 0; };
	CAdPcmBase* GetPCMDeviceFromIndex(UINT8 i) const { return theConfig ? theConfig->GetPCMDeviceFromIndex(i) : 0; };
	BOOL isPcmDevice(UINT32 uid) { return theConfig ? theConfig->isPcmDevice(uid) : 0; };
	BOOL isPcmDevice(CSoundDevice* pdev) { return theConfig ? theConfig->isPcmDevice(pdev) : 0; };
	UINT32 GetDeviceUniqID(CSoundDevice* pdev) { return theConfig ? theConfig->GetDeviceUniqID(pdev) : 0; };
	UINT32 GetDeviceUniqID(CAdPcmBase* pdev) { return theConfig ? theConfig->GetDeviceUniqID(pdev) : 0; };
	CSoundDevice* GetDeviceFromUniqID(UINT32 uid) { return theConfig ? theConfig->GetDeviceFromUniqID(uid) : 0; };
	int SetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 num);
	void ReloadVoice(FMVOICE* voice, UINT32 dev, UINT8 bank, UINT8 num);
	void SetMasterVolume(UINT8 vol) { theConfig ? theConfig->SetMasterVolume(vol) : void(0); };
	UINT8 GetMasterVolume() { return theConfig ? theConfig->GetMasterVolume() : 0; };
	void GetWaveform(SCCWAVE* dst, int num) { theConfig ? theConfig->GetWaveform(dst, num) : void(0); };
	void SetLCDdot(int page, int x, int y, int val);
	void SetLCDrow(int page, int r, BYTE src[16]);
	void SetLCDcol(int page, int c, BYTE src[16]);
	void SetLCDall(int page, BYTE lcd[16][16]);
	void SetLCDpage(int page);
	void SetLCDstr(char src[17]);
	BYTE GetLCDdot(int x, int y);
	void GetLCDrow(BYTE dst[16], int r);
	void GetLCDcol(BYTE dst[16], int c);
	void GetLCDall(BYTE dst[16][16]);
	void GetLCDstr(char dst[17]);


	//Callback
	int PollingCallBack();
	void TimerCallBack(UINT32 tick);
	void MIDIClockCallBack();

	//Utility
	void AllNoteOff();
	void ResetAllCtrl();

	//Static utils
	static const DWORD GetDeviceVoiceType(DWORD device);
	static const DWORD GetDeviceVoiceGroupMask(DWORD device);
	static const DWORD GetDeviceIDFromName(LPCTSTR name);
	static const TCHAR* GetDeviceNameFromID(DWORD devid);
	static const DWORD GetDeviceRegSize(DWORD devid);
	static DWORD* GetCompatiList(DWORD devid);
	static int GetClockCode(UINT32 clock);
private:
	CFITOM();
	CFITOM& operator=(CFITOM& rhs);
	CFITOM(CFITOM& rhs);
	~CFITOM();
};

#endif //__FMIDI98_H__
