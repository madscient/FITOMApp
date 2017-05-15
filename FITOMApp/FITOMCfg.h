#pragma once

struct FMVOICE;

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <queue>

#define LLC_CMD_NOP   0x1000
#define LLC_CMD_NOTE  0x1001
#define LLC_CMD_CENT  0x1002
#define LLC_CMD_VEL   0x1003
#define LLC_CMD_WAIT  0x10ff
#define LLC_CMD_TONE  0x2000
#define LLC_CMD_FREQ  0x2001
#define LLC_CMD_NOISE 0x2010
#define LLC_CMD_NFREQ 0x2011
#define LLC_CMD_NAM   0x2012
#define LLC_CMD_NOM   0x2013
#define LLC_CMD_AMP   0x2020
#define LLC_CMD_AMPL  0x2021
#define LLC_CMD_AMPR  0x2022
#define LLC_CMD_ENVM  0x2030
#define LLC_CMD_ENVF  0x2031
#define LLC_CMD_ENVS  0x2032
#define LLC_CMD_ENVP  0x2033
#define LLC_CMD_ENVR  0x2034

class CMasterVolumeCtrl;

class CFITOMDeviceConfig {
public:
	CFITOMDeviceConfig(LPCTSTR cfgstr);
	~CFITOMDeviceConfig() {};
	DWORD ifid; // Interface ID (on SCCI)
	DWORD slid; // Slot ID (on SCCI)
	DWORD devid; // Device ID (on SCCI)
	DWORD mode; // Specific mode (on FITOM)
	DWORD clock; // Clock ID (on FITOM)
	DWORD pan; // Panpot (0:Stereo, 1:Monaural-L, 2:Monaural-R, 3:Monaural-LR, 4:Stereo-reverse)
	DWORD mtype; // Motherboard type
	DWORD dd; //Device dependent parameter
};

struct DRUMMAP {
	CSoundDevice* device;
	TCHAR name[16];
	UINT32 devID;
	UINT8 bank;
	UINT8 prog;
	SINT8 pan;
	UINT8 num;
	UINT16 fnum;
	UINT16 gate;
};

struct SCCWAVE {
	TCHAR name[16];
	SINT8 wave[32];
};

class CAdPcmBase;
struct PCMPROG {
	TCHAR progname[32];
	int hirate;
	CAdPcmBase* parent;
};

template < typename T > class CVoiceBankTemplate {
protected:
	T* vbank;
	TCHAR* bankname;
	TCHAR* filename;
public:
	CVoiceBankTemplate();
	~CVoiceBankTemplate();
	virtual void SetBankName(LPCTSTR bname);
	virtual void SetFileName(LPCTSTR fname);
	virtual const TCHAR* GetBankName() { return (const TCHAR*)bankname; };
	virtual const TCHAR* GetFileName() { return (const TCHAR*)filename; };
	virtual void GetVoice(int prog, T* voice);
	virtual void SetVoice(int prog, const T* voice);
};

class CFMBank : public CVoiceBankTemplate < FMVOICE > {
public:
	CFMBank();
	void Renumber(int grp, int bnk);
};

class CPcmBank : public CVoiceBankTemplate < PCMPROG > {
public:
	CPcmBank();
};

class CDrumBank : public CVoiceBankTemplate < DRUMMAP > {
public:
	CDrumBank();
};

class CMidiIn;

struct ChannelMap {
	int device_id;
	int polyphon;
};

struct PERFCMD {
	UINT32	cmd;
	SINT32	param;
};

class CLLPBank {
protected:
	UINT32 devid;
	CSoundDevice* pDevice;
	std::queue<PERFCMD> cmdlist;
public:
	CLLPBank();
	int LoadPerfCmd(LPCTSTR str);
	PERFCMD front();
	PERFCMD pop();
};

class CFITOMConfig {
protected:
	boost::property_tree::tiptree fitom_ini;
	CMasterVolumeCtrl* pMasVol;
	CMidiIn* vMidiIn[MAX_MPUS];
	//CMidiInst* vMidiInst[MAX_MPUS];
	CSoundDevice* vPhyDev[MAX_DEVS];
	CSoundDevice* vLogDev[MAX_DEVS];
	CAdPcmBase* vPcmDev[MAX_DEVS];
	CFMBank* vOpmBank[MAX_BANK];
	CFMBank* vOpnBank[MAX_BANK];
	CFMBank* vOpl2Bank[MAX_BANK];
	CFMBank* vOpl3Bank[MAX_BANK];
	CFMBank* vOpllBank[MAX_BANK];
	CFMBank* vPsgBank[MAX_BANK];
	CPcmBank* vPcmBank[MAX_BANK];
	CDrumBank* vDrumBank[MAX_BANK];
	CLLPBank* vPerfBank[128];
	SCCWAVE vSccWaveForm[64];
	int phydevs;
	int logdevs;
	int pcmdevs;
	int mpus;
	int MIDIClockCh;
	ChannelMap channelMap[16];
	struct FITOMDEV {
		UINT32 uniqID;
		CSoundDevice* pDevice;
	};
	void(*pProgressMessage)(LPCTSTR str);
	void(*pProgressFilename)(LPCTSTR str);
	int LoadMidiConfig();
	int LoadDeviceConfig();
	int LoadVoiceConfig();
	int LoadADPCMConfig();
	int ParseVoiceBank(int groupcode);
	int ParseRhythmBank();
	int LoadDrumBank(CDrumBank* bank, LPCTSTR fname);
	int LoadADPCMBank(int bank, LPCTSTR fname);
	int LoadSCCWaveBank();
	int LoadLowLevelPerfBank();

public:
	CFITOMConfig(LPCTSTR strinifile);
	~CFITOMConfig();
	int LoadConfig();
	CAdPcmBase* AddDevice(CAdPcmBase* pdev);
	CSoundDevice* AddDevice(CSoundDevice* pdev);
	int AddDevice(CMidiIn* pmpu);
	virtual CPort* CreatePort(int devtype, LPCTSTR params) = 0;
	virtual CMidiIn* CreateMidiInPort(LPCTSTR params) = 0;
	virtual int CreateDevice(LPCTSTR param);
	virtual CMasterVolumeCtrl* CreateMasVol(LPCTSTR param) = 0;
	int CreateSingleDevice(int devtype, LPCTSTR param);
	int CreateADPCMDevice(int devtype, LPCTSTR param);
	CFMBank* AllocFMBank(int voicegroup, int bank);
	CFMBank* GetFMBank(int voicegroup, int bank);
	CDrumBank* AllocDrumBank(int prog);
	CDrumBank* GetDrumBank(int prog);
	CPcmBank* AllocPcmBank(int prog);
	CPcmBank* GetPcmBank(int prog);
	CLLPBank* GetLowLevel(int perf);
	void GetWaveform(SCCWAVE* dst, int num);
	int GetDeviceName(UINT32 devid, TCHAR* name, size_t count);
	int GetVoiceName(UINT32 devid, UINT32 bank, UINT32 prog, TCHAR* name, size_t count);
	const TCHAR* GetBankName(UINT32 devid, UINT32 bank);
	const UINT8 GetMidiInputs() const { return mpus; };
	const UINT8 GetMidiOutputs() const { return 0; };
	const UINT8 GetLogDevs() const { return logdevs; };
	const UINT8 GetPhyDevs() const { return phydevs; };
	const UINT8 GetPcmDevs() const { return pcmdevs; };
	CMidiIn* GetMidiIn(UINT8 i) const { return (i<mpus) ? vMidiIn[i] : NULL; };
	CMidiOut* GetMidiOut(UINT8 i) const { return NULL; };
	//CMidiInst* GetMidiInst(UINT8 i) const { return (i<mpus) ? vMidiInst[i] : NULL; };
	CSoundDevice* GetLogDeviceFromIndex(UINT8 i) const { return (i<logdevs) ? vLogDev[i] : NULL; };
	CSoundDevice* GetPhysDeviceFromIndex(UINT8 i) const { return (i<phydevs) ? vPhyDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromIndex(UINT8 i) const { return (i<pcmdevs) ? vPcmDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromID(UINT32 id);
	CSoundDevice* GetPhysDeviceFromID(UINT32 id) const;
	CSoundDevice* GetLogDeviceFromID(UINT8 devid) const;
	const int GetLogDeviceIDFromIndex(UINT8 i) const;
	int GetLogDeviceIndex(CSoundDevice* pdev);
	int GetLogDeviceIndex(UINT8 devid);
	int GetPcmDeviceIndex(CAdPcmBase* pdev);
	int GetPcmDeviceIndex(UINT32 devid);
	CSoundDevice* GetDeviceFromUniqID(UINT32 uid);
	UINT32 GetDeviceUniqID(CSoundDevice* pdev, int phys = 0);
	UINT32 GetDeviceUniqID(CAdPcmBase* pdev);
	BOOL isPcmDevice(UINT32 uid) { return GetPCMDeviceFromID(uid) != 0; };
	BOOL isPcmDevice(CSoundDevice* pdev) { return GetPcmDeviceIndex((CAdPcmBase*)pdev) >= 0; };
	void SetChannelMap(int ch, int devid, int pol) { if (ch < 16) { channelMap[ch].device_id = devid; channelMap[ch].polyphon = pol; } };
	ChannelMap* const GetChannelMap(int ch) { return (ch < 16) ? &channelMap[ch] : 0; };
	int GetChannelMapDevice(int ch) { return (ch < 16) ? channelMap[ch].device_id : -1; };
	int GetChannelMapPoly(int ch) { return (ch < 16) ? channelMap[ch].polyphon : 0; };
	void SetMIDIClockCh(int ch) { MIDIClockCh = ch; };
	int GetMIDIClockCh() { return MIDIClockCh; };
	void SetProgressMsgFunc(void(*pFunc)(LPCTSTR str)) { pProgressMessage = pFunc; };
	void SetProgressFileFunc(void(*pFunc)(LPCTSTR str)) { pProgressFilename = pFunc; };
	void SetMasterVolume(UINT8 vol);
	UINT8 GetMasterVolume();

	int ParseOPMVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPNVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPL2Voice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPL3Voice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPLLVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParsePSGVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseLFOParam(FMVOICE* voice, int index, std::vector<int>& param);

	int BuildOPMVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPNVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPL2Voice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPL3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPLLVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildPSGVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildLFOParam(FMVOICE* voice, int index, TCHAR* result, size_t length);
};

class CFITOMConfigWin32 : public CFITOMConfig {
protected:
	CSCCIWrapper* pScci;
	int g_devno;
public:
	CFITOMConfigWin32(LPCTSTR strinifile);
	~CFITOMConfigWin32();
	virtual CPort* CreatePort(int devtype, LPCTSTR params);
	virtual CMidiIn* CreateMidiInPort(LPCTSTR params);
	virtual CMasterVolumeCtrl* CreateMasVol(LPCTSTR param);
	CSCCIWrapper* GetScci() { return pScci; };
};
