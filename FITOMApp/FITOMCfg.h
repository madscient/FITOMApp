#pragma once

struct FMVOICE;

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <queue>

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
	uint32_t devID;
	int32_t	ch;
	uint8_t bank;
	uint8_t prog;
	int8_t pan;
	uint8_t num;
	uint16_t fnum;
	uint16_t gate;
};

struct SCCWAVE {
	TCHAR name[16];
	int8_t wave[32];
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

struct PortInfo {
	TCHAR ifname[16];
	DWORD ifid;
	DWORD slid;
	CPort* port;
};

class COPNA;
class COPK;
class COPLL;
class COPN3L;

class CFITOMConfig {
protected:
	boost::property_tree::tiptree fitom_ini;
	CMasterVolumeCtrl* pMasVol;
	CMidiIn* vMidiIn[MAX_MPUS];
	//CMidiInst* vMidiInst[MAX_MPUS];
	std::vector<CSoundDevice*> vPhyDev;
	std::vector<CSoundDevice*> vLogDev;
	std::vector<CAdPcmBase*> vPcmDev;
	std::vector<PortInfo> vPort;
	//CSoundDevice* vPhyDev[MAX_DEVS];
	//CSoundDevice* vLogDev[MAX_DEVS];
	//CAdPcmBase* vPcmDev[MAX_DEVS];
	CFMBank* vOpmBank[MAX_BANK];	//OPM/OPP/OPZ
	CFMBank* vOpnBank[MAX_BANK];	//OPN/OPNA/OPN2/etc...
	CFMBank* vOpl2Bank[MAX_BANK];	//OPL/OPL2/MA-2(2OP)/etc...
	CFMBank* vOpl3Bank[MAX_BANK];	//OPL3/MA-2(4OP)
	CFMBank* vOpllBank[MAX_BANK];	//OPLL family
	CFMBank* vMAFMBank[MAX_BANK];	//MA-3/MA-5/SD-1
	CFMBank* vPsgBank[MAX_BANK];
	CPcmBank* vPcmBank[MAX_BANK];
	CDrumBank* vDrumBank[MAX_BANK];
	SCCWAVE vSccWaveForm[64];
	int mpus;
	int MIDIClockCh;
	ChannelMap channelMap[16];
	struct FITOMDEV {
		uint32_t uniqID;
		CSoundDevice* pDevice;
	};
	uint32_t UsingVoiceGroup;
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
	CPort* FindPort(PortInfo& pinf);
	CSoundDevice* FindDeviceFromPort(CPort* pt);
	int BuildLogDevice();
	virtual int isSpannable(CSoundDevice* src, CSoundDevice* tgt);
	virtual int AutoDeviceConfig() = 0;
	virtual int ManualDeviceConfig() = 0;

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
	virtual void TimerCallBack() = 0;
	int CreateSingleDevice(int devtype, LPCTSTR param);
	int CreateADPCMDevice(int devtype, LPCTSTR param);
	CFMBank* AllocFMBank(uint32_t voicegroup, uint32_t bank);
	CFMBank* GetFMBank(uint32_t voicegroup, uint32_t bank);
	CDrumBank* AllocDrumBank(int prog);
	CDrumBank* GetDrumBank(int prog);
	CPcmBank* AllocPcmBank(int prog);
	CPcmBank* GetPcmBank(int prog);
	void GetWaveform(SCCWAVE* dst, int num);
	int GetDeviceName(uint32_t devid, TCHAR* name, size_t count);
	int GetVoiceName(uint32_t devid, uint32_t bank, uint32_t prog, TCHAR* name, size_t count);
	const TCHAR* GetBankName(uint32_t devid, uint32_t bank);
	const uint8_t GetMidiInputs() const { return mpus; };
	const uint8_t GetMidiOutputs() const { return 0; };
	const uint8_t GetLogDevs() const { return vLogDev.size(); };
	const uint8_t GetPhyDevs() const { return vPhyDev.size(); };
	const uint8_t GetPcmDevs() const { return vPcmDev.size(); };
	CMidiIn* GetMidiIn(uint8_t i) const { return (i < mpus) ? vMidiIn[i] : NULL; };
	CMidiOut* GetMidiOut(uint8_t i) const { return NULL; };
	//CMidiInst* GetMidiInst(uint8_t i) const { return (i<mpus) ? vMidiInst[i] : NULL; };
	CSoundDevice* GetLogDeviceFromIndex(uint8_t i) const { return (i < vLogDev.size()) ? vLogDev[i] : NULL; };
	CSoundDevice* GetPhysDeviceFromIndex(uint8_t i) const { return (i < vPhyDev.size()) ? vPhyDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromIndex(uint8_t i) const { return (i < vPcmDev.size()) ? vPcmDev[i] : NULL; };
	CAdPcmBase* GetPCMDeviceFromID(uint32_t id);
	CSoundDevice* GetPhysDeviceFromID(uint32_t id) const;
	CSoundDevice* GetLogDeviceFromID(uint8_t devid) const;
	const int GetLogDeviceIDFromIndex(uint8_t i) const;
	int GetLogDeviceIndex(CSoundDevice* pdev);
	int GetLogDeviceIndex(uint8_t devid);
	int GetPcmDeviceIndex(CAdPcmBase* pdev);
	int GetPcmDeviceIndex(uint32_t devid);
	CSoundDevice* GetDeviceFromUniqID(uint32_t uid);
	uint32_t GetDeviceUniqID(CSoundDevice* pdev, int phys = 0);
	uint32_t GetDeviceUniqID(CAdPcmBase* pdev);
	BOOL isPcmDevice(uint32_t uid) { return GetPCMDeviceFromID(uid) != 0; };
	BOOL isPcmDevice(CSoundDevice* pdev) { return GetPcmDeviceIndex((CAdPcmBase*)pdev) >= 0; };
	void SetChannelMap(int ch, int devid, int pol) { if (ch < 16) { channelMap[ch].device_id = devid; channelMap[ch].polyphon = pol; } };
	ChannelMap* const GetChannelMap(int ch) { return (ch < 16) ? &channelMap[ch] : 0; };
	int GetChannelMapDevice(int ch) { return (ch < 16) ? channelMap[ch].device_id : -1; };
	int GetChannelMapPoly(int ch) { return (ch < 16) ? channelMap[ch].polyphon : 0; };
	void SetMIDIClockCh(int ch) { MIDIClockCh = ch; };
	int GetMIDIClockCh() { return MIDIClockCh; };
	void SetProgressMsgFunc(void(*pFunc)(LPCTSTR str)) { pProgressMessage = pFunc; };
	void SetProgressFileFunc(void(*pFunc)(LPCTSTR str)) { pProgressFilename = pFunc; };
	void SetMasterVolume(uint8_t vol);
	uint8_t GetMasterVolume();

	int ParseOPMVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPNVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPL2Voice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPL3Voice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseOPLLVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseMA3Voice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParsePSGVoice(FMVOICE* voice, int index, std::vector<int>& param);
	int ParseLFOParam(FMVOICE* voice, int index, std::vector<int>& param);

	int BuildOPMVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPNVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPL2Voice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPL3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildOPLLVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildMA3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildPSGVoice(FMVOICE* voice, int index, TCHAR* result, size_t length);
	int BuildLFOParam(FMVOICE* voice, int index, TCHAR* result, size_t length);

	int AddOPN(CPort* pt, int md, int fs);
	int AddOPN2(CPort* pt, int md, int fs);
	int AddOPN3L(CPort* pt, int md, int fs);
	int AddOPNA(CPort* pt, int md, int fs);
	int AddOPNARhythm(COPNA* pOpna);
	int AddOPNARhythm(CPort* pt, int md, int fs);
	int AddOPN3LRhythm(COPN3L* pOpn3l);
	int AddOPN3LRhythm(CPort* pt, int md, int fs);
	int AddOPNB(CPort* pt, int md, int fs);
	int AddOPM(CPort* pt, int md, int fs);
	int AddOPP(CPort* pt, int md, int fs);
	int AddOPZ(CPort* pt, int md, int fs);
	int AddOPL(CPort* pt, int md, int fs);
	int AddOPL2(CPort* pt, int md, int fs);
	int AddOPL3(CPort* pt, int md, int fs);
	int AddOPL3_2(CPort* pt, int md, int fs);
	int AddOPLL(CPort* pt, int md, int fs);
	int AddOPLL2(CPort* pt, int md, int fs);
	int AddOPLLP(CPort* pt, int md, int fs);
	int AddOPLLX(CPort* pt, int md, int fs);
	int AddOPLLRhythm(CPort* pt, int md, int fs);
	int AddOPLLRhythm(COPLL* pOpll);
	int AddOPK(CPort* pt, int md, int fs);
	int AddOPK2(CPort* pt, int md, int fs);
	int AddOPKRhythm(CPort* pt, int md, int fs);
	int AddOPKRhythm(COPK* pOpk);
	int AddSSG(CPort* pt, int md, int fs);
	int AddSSGLP(CPort* pt, int md, int fs);
	int AddSSGD(CPort* pt, int md, int fs);
	int AddPSG(CPort* pt, int md, int fs);
	int AddEPSG(CPort* pt, int md, int fs);
	int AddDCSG(CPort* pt, int md, int fs);
	int AddDSG(CPort* pt, int md, int fs);
	int AddSCC(CPort* pt, int md, int fs);
	int AddSCCP(CPort* pt, int md, int fs);
	int AddSD1(CPort* pt, int md, int fs);
};

class CFTSPI;
class CFITOMConfigWin32 : public CFITOMConfig {
protected:
	CSCCIWrapper* pScci;
	CFTSPI* pFtspi;
	int g_devno;
	virtual int AutoDeviceConfig();
	virtual int ManualDeviceConfig();
public:
	CFITOMConfigWin32(LPCTSTR strinifile);
	~CFITOMConfigWin32();
	virtual void TimerCallBack();
	virtual CPort* CreatePort(int devtype, LPCTSTR params);
	virtual CMidiIn* CreateMidiInPort(LPCTSTR params);
	virtual CMasterVolumeCtrl* CreateMasVol(LPCTSTR param);
	virtual int CreateSCCIDevice(int ifid, int slid);
	CPort* CreateSCCIPort(int ifid, int slid, int devtype);
	CSCCIWrapper* GetScci() { return pScci; };
	CFTSPI* GetFTspi() { return pFtspi; };
};

