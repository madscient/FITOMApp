#ifndef __FMIDI98_H__
#define __FMIDI98_H__

//Resource code
#define	DEVICE_NONE	0
#define	DEVICE_SSG	1	//YM2149
#define	DEVICE_OPN	2	//YM2203
#define	DEVICE_OPN2	3	//YM2612
#define	DEVICE_OPNA	4	//YM2608
#define	DEVICE_OPM	5	//YM2151
#define	DEVICE_OPLL	6	//YM2413
#define	DEVICE_OPL	7	//YM3526
#define	DEVICE_OPL2	8	//YM3812
#define	DEVICE_OPL3	9	//YMF262
#define DEVICE_OPN3L 10	//YMF288
#define DEVICE_OPNB 11	//YM2610
#define DEVICE_SAA	12	//SAA1099
#define DEVICE_DSG	13	//YM2163
#define	DEVICE_PSG	15	//AY-3-891x
#define	DEVICE_DCSG	16	//SN76489
#define	DEVICE_SCC	17	//SCC with ROM
#define DEVICE_SCCP	18	//SCC for SNATCHER
#define	DEVICE_SSGS	19	//YMZ705
#define	DEVICE_EPSG	20	//AY8930
#define DEVICE_SSGL 21	//YMZ284
#define DEVICE_SSGLP 22	//YMZ294
#define DEVICE_SID	23	//MOS6581/8580
#define	DEVICE_Y8950	24	//YM3801 a.k.a. MSX-AUDIO
#define	DEVICE_OPL3_2	25	//2op ch of OPL3
#define	DEVICE_OPP	26	//YM2164
#define	DEVICE_OPZ	27	//YM2414
#define	DEVICE_OPZ2	28	//YM2424
#define	DEVICE_OPLLP	29	//YMF281
#define	DEVICE_OPLL2	30	//YM2420
#define DEVICE_OPNC 31	//YMF264
#define DEVICE_OPN2C 32	//YM3438
#define DEVICE_OPN2L 33	//YMF276
#define DEVICE_2610B 34	//YM2610B
#define DEVICE_F286 35	//YMF286-K
#define DEVICE_OPN3 36	//YMF297
#define DEVICE_OPN3_L3 37	//OPL mode of YMF297
#define DEVICE_OPN3_N3 38	//OPN mode of YMF297
#define DEVICE_OPLLX	39	//YM2423/MS1823
#define DEVICE_OPK 40	//YM7116
#define DEVICE_OPK2 41	//YM7129
#define DEVICE_OPQ	42	//YM3806/YM3533
#define DEVICE_RYP4	43	//YM2154
#define DEVICE_RYP6	44	//YM3301/YM3302
#define DEVICE_FMS	45	//YMZ735/YMZ736
#define DEVICE_5232	47	//MSM5232
#define DEVICE_OPL4	48	//YMF278 with YRW801
#define DEVICE_OPL4ML	49	//YMF704
#define DEVICE_OPL4ML2	50	//YMF721
#define DEVICE_MA1		51	//YMU757
#define DEVICE_MA2		52	//YMU759
#define DEVICE_MA3		53	//YMU762
#define DEVICE_MA5		54	//YMU765
#define DEVICE_MA7		55	//YMU768
#define DEVICE_SD1		56	//YMF825
#define DEVICE_PCMD8	65	//YMZ280

#define	DEVICE_ADPCM	119	//virtual device for ADPCM channel
#define	DEVICE_ADPCMA	118	//YM2610/YM2610B
#define	DEVICE_ADPCMB	117	//YM2608/YM2610/YM2610B/YM3801

#define DEVICE_RHYTHM	120	// Virtual device for rhythm channel

#define DEVICE_MULTI	1024	//Multiple module
#define DEVICE_NBV		1025	//NBV3/NBV4
#define	DEVICE_AYB		1026	//AYB01/AYB02

#define VOICE_TYPE_OPM  0x10
#define VOICE_TYPE_OPN  0x11
#define VOICE_TYPE_OPNA	 0x12
#define VOICE_TYPE_OPZ  0x13

#define VOICE_TYPE_OPL2 0x20
#define VOICE_TYPE_OPL  0x21
#define VOICE_TYPE_OPLL 0x22
#define VOICE_TYPE_OPK	0x23

#define VOICE_TYPE_OPL3 0x30
#define VOICE_TYPE_MA3	0x31

#define VOICE_TYPE_SSG  0x40
#define VOICE_TYPE_EPSG 0x41
#define VOICE_TYPE_DCSG 0x42
#define VOICE_TYPE_DSG	0x43
#define VOICE_TYPE_SAA	0x44
#define VOICE_TYPE_SCC	0x48

#define VOICE_TYPE_DRUM 0x50
#define VOICE_TYPE_PCM	0x70
#define VOICE_TYPE_OPL4 0x71

#define VOICE_GROUP_NONE 0x00
#define VOICE_GROUP_OPM  0x01
#define VOICE_GROUP_OPNA 0x02
#define VOICE_GROUP_OPL2 0x04
#define VOICE_GROUP_OPL3 0x08
#define VOICE_GROUP_OPLL 0x10
#define VOICE_GROUP_PSG  0x20
#define VOICE_GROUP_OPL4 0x40
#define VOICE_GROUP_PCM	 0x80
#define VOICE_GROUP_MA3  0x100
#define VOICE_GROUP_RHYTHM 0x8000
#define VOICE_GROUP_ALL  0xffff

#define DEVICE_CAP_NONE 0x00
#define DEVICE_CAP_FM   0x01
#define DEVICE_CAP_PSG  0x02
#define DEVICE_CAP_RHY  0x04
#define DEVICE_CAP_PCM  0x08

#define RHYTHM_BANK	DEVICE_RHYTHM
#define ADPCM_BANK	0x77

#define CLKCODE358	0
#define CLKCODE383	4
#define CLKCODE399	1
#define CLKCODE400	2
#define CLKCODE409	3

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
	CFMBank* GetFMBank(UINT8 voiceid, UINT8 bank);
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
	static const UINT8 GetDeviceVoiceType(UINT8 device);
	static const UINT8 GetDeviceVoiceGroupMask(UINT8 device);
	static const UINT8 GetDeviceIDFromName(LPCTSTR name);
	static const TCHAR* GetDeviceNameFromID(UINT8 devid);
	static const DWORD GetDeviceRegSize(UINT8 devid);
	static UINT8* GetCompatiList(UINT8 devid);
	static int GetClockCode(UINT32 clock);
private:
	CFITOM();
	CFITOM& operator=(CFITOM& rhs);
	CFITOM(CFITOM& rhs);
	~CFITOM();
};

#endif //__FMIDI98_H__
