#ifndef __FMIDI98_H__
#define __FMIDI98_H__

#define MAX_BANK 10
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
	uint16_t addr;
	uint16_t data;
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
	uint32_t MIDItick;
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
	int GetVoice(FMVOICE* voice, uint8_t dev, uint8_t bank, uint8_t num);
	int GetDrum(DRUMMAP* drum, uint8_t bank, uint8_t prog, uint8_t note);
	CMidiInst* GetMidiInst(uint8_t i) const { return (i<theConfig->GetMidiInputs()) ? midi[i] : NULL; };
	CFMBank* GetFMBank(uint32_t voiceid, uint32_t bank);
	CDrumBank* GetDrumBank(uint8_t prog);
	CPcmBank* GetPcmBank(uint8_t bank);

	const uint8_t GetLogDevs() const { return theConfig ? theConfig->GetLogDevs() : 0; };
	const uint8_t GetPhyDevs() const { return theConfig ? theConfig->GetPhyDevs() : 0; };
	const uint8_t GetPcmDevs() const { return theConfig ? theConfig->GetPcmDevs() : 0; };
	CSoundDevice* GetLogicalDeviceFromID(uint8_t devid) const { return theConfig ? theConfig->GetLogDeviceFromID(devid) : 0; };
	CSoundDevice* GetLogicalDeviceFromIndex(uint8_t i) const { return theConfig ? theConfig->GetLogDeviceFromIndex(i) : 0; };
	CAdPcmBase* GetPCMDeviceFromIndex(uint8_t i) const { return theConfig ? theConfig->GetPCMDeviceFromIndex(i) : 0; };
	BOOL isPcmDevice(uint32_t uid) { return theConfig ? theConfig->isPcmDevice(uid) : 0; };
	BOOL isPcmDevice(CSoundDevice* pdev) { return theConfig ? theConfig->isPcmDevice(pdev) : 0; };
	uint32_t GetDeviceUniqID(CSoundDevice* pdev) { return theConfig ? theConfig->GetDeviceUniqID(pdev) : 0; };
	uint32_t GetDeviceUniqID(CAdPcmBase* pdev) { return theConfig ? theConfig->GetDeviceUniqID(pdev) : 0; };
	CSoundDevice* GetDeviceFromUniqID(uint32_t uid) { return theConfig ? theConfig->GetDeviceFromUniqID(uid) : 0; };
	int SetVoice(FMVOICE* voice, uint8_t dev, uint8_t bank, uint8_t num);
	void ReloadVoice(FMVOICE* voice, uint32_t dev, uint8_t bank, uint8_t num);
	void SetMasterVolume(uint8_t vol) { theConfig ? theConfig->SetMasterVolume(vol) : void(0); };
	uint8_t GetMasterVolume() { return theConfig ? theConfig->GetMasterVolume() : 0; };
	void GetWaveform(SCCWAVE* dst, int num) { theConfig ? theConfig->GetWaveform(dst, num) : void(0); };

	//Callback
	int PollingCallBack();
	void TimerCallBack(uint32_t tick);
	void MIDIClockCallBack();

	//Utility
	void AllNoteOff();
	void ResetAllCtrl();

	//Command shell
	typedef BOOL(CFITOM::*cmdfunc)(TCHAR* cmd, TCHAR* result, int reslen);

	typedef struct {
		const char* cmd;
		cmdfunc func;
	} cmdvect;
	BOOL CmdProc(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdSetVoice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetVoice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetMidiStatus(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetDeviceStatus(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdSendMidi(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetPhysicalDevice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetLogicalDevice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetMidiDevice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdSetPcmVoice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetPcmVoice(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdSetWaveForm(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetWaveForm(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdSetDeviceReg(TCHAR* cmd, TCHAR* result, int reslen);
	BOOL cmdGetDeviceReg(TCHAR* cmd, TCHAR* result, int reslen);

	//Static utils
	static const DWORD GetDeviceVoiceType(DWORD device);
	static const DWORD GetDeviceVoiceGroupMask(DWORD device);
	static const DWORD GetDeviceIDFromName(LPCTSTR name);
	static const TCHAR* GetDeviceNameFromID(DWORD devid);
	static const DWORD GetDeviceRegSize(DWORD devid);
	static DWORD* GetCompatiList(DWORD devid);
	static int GetClockCode(uint32_t clock);

private:
	static cmdvect commandtable[];
	CFITOM();
	CFITOM& operator=(CFITOM& rhs);
	CFITOM(CFITOM& rhs);
	~CFITOM();
};

#endif //__FMIDI98_H__
