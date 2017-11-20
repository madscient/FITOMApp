#ifndef __FMCHIP_H__
#define __FMCHIP_H__

#include "Port.h"
#include "Fnum.h"

#define FNUM_OFFSET	(-576)	//Fnum算出基点(440Hz)とノートNo基点(60=O4C)との差分

#pragma pack(1)

struct FMOP {
	//EG parameter
	UINT8	AR;  // 00:Attack Rate: 4bit OPL / 5bit OPN/OPM
	UINT8	DR;  // 01:Decay Rate: 4bit OPL / 5bit OPN/OPM
	UINT8	SL;  // 02:Sustain Level: 4bit OPL/OPN/OPM
	UINT8	SR;  // 03:Sustain Rate: 1bit OPL / 5bit OPN/OPM
	UINT8	RR;  // 04:Release Rate: 4bit OPL/OPN/OPM
	UINT8	REV; // 05:Reverberation: OPZ / SRR sustain release rate
	UINT8	TL;  // 06:Total Level: 6bit OPL / 7bit OPN/OPM
	UINT8	EGT; // 07:SSG EG type: 4bit OPN
	UINT8	EGS; // 08:EG bias: OPZ
	UINT8	KSL; // 09:Key Scale Level: 2bit OPL / none OPN/OPM
	UINT8	KSR; // 10:Key Scale Rate: 1bit OPL / 2bit OPN/OPM
	UINT8	WS;  // 11:Wave Select: 1bit OPLL / 2bit OPL2 / 3bit OPL3 / none OPN/OPM / 3bit OPZ
	//LFO parameter
	UINT8	AM;  // 12:AM enable: 1bit OPL/OPNA/OPM
	UINT8	VIB; // 13:Vibrato: 1bit OPL/OPNA
	UINT8	SLF; // 14:Soft LFO frequency 0-15
	UINT8	SLW; // 15:Soft LFO waveform 0:up saw/1:square/2:delta/3:s&h/4:down saw1/5:delta1/6:sin
	UINT8	SLD; // 16:Soft LFO depth: 0 to 63, 64-127 for -64 to -1
	UINT8	SLY; // 17:Soft LFO delay: 0-127 in 20ms unit
	UINT8	SLR; // 18:Soft LFO rate: 0-127
	//Osc frequency
	UINT8	DM0; // 19:Oscilator fixed freq (OPZ)
	UINT8	MUL; // 20:Multiple: 4bit OPL/OPN/OPM
	UINT8	DT1; // 21:Detune1: none OPL / 4bit OPN/OPM
	UINT8	DT2; // 22:Detune2: none OPL/OPN / 3bit OPM
	UINT8	DT3; // 23:fine frequency: 4bit OPZ ratio mode
};

struct FMVOICE {
	FMVOICE() : ID(0xffffffffUL) { name[0] = '\0'; };
	UINT32	ID;  //00:ID
	char	name[16];//04:name
	UINT8	FB;  // 20:Feedback: 3bit OPL/OPN/OPM
	UINT8	AL;  // 21:Algorythm: 1bit OPL / 4bit OPL3 / 3bit OPN/OPM/MA3
	UINT8	AMS; // 22:AM Sensitivity: none OPL / 2bit OPM
	UINT8	PMS; // 23:PM Sensitivity: none OPL / 3bit OPM
	UINT8	LDM; // 24:LFO Depth MSB: 0 to 8191, 8192-16383 for -8192 to -1
	UINT8	LDL; // 25:LFO Depth LSB: 0 to 8191, 8192-16383 for -8192 to -1
	UINT8	LFO; // 26:LFO frequency
	UINT8	LWF; // 27:LFO Wave form 0:up saw/1:square/2:delta/3:s&h/4:down saw1/5:delta1/6:sin
	UINT8	LFS; // 28:LFO sync: HW LFO sync flag
	UINT8	LFD; // 29:LFO delay: 0-127 in 20ms unit
	UINT8	LFR; // 30:LFO rate: 0-127
	UINT8	NFQ; // 31:Noise frequency: 5bit OPM/OPP/OPZ/SSG | H-LFO freq: 2bit MA3
	FMOP	op[4];
};

#pragma pack()

class CMidiCh;
extern SINT8 GetLFOWave(UINT8 waveform, UINT8 speed, UINT16 phase);
extern UINT8 CalcVolExpVel(int vol, int exp, int vel);
extern UINT8 CalcEffectiveLevel(UINT8 vev, UINT8 tl);
extern UINT8 CalcLinearLevel(UINT8 vev, UINT8 tl);
extern UINT8 Linear2dB(UINT8 evol, int range, int step, int bw);

#define	RANGE96DB	0
#define	RANGE48DB	1
#define	RANGE24DB	2
#define	RANGE12DB	3
#define STEP075DB	0x7f
#define STEP150DB	0x7e
#define STEP300DB	0x7c
#define STEP600DB	0x78

class ISoundDevice {
public:
	struct FNUM {
		FNUM();
		FNUM(UINT8 bl, UINT16 fn);
		UINT8	block;	// Block number/Key code(OPM)
		UINT16	fnum;	// F-number/Key fractal(OPM)
	};
protected:
	class CEnvelope {
	public:
		enum EG_PHASE { EG_ATTACK, EG_DECAY, EG_SUSTAIN, EG_RELEASE, EG_NONE, };
		CEnvelope();
		int Update();
		void Start(const FMOP* env);
		void Stop();
		void Release();
		UINT8 GetValue() { return UINT8(value); };
		EG_PHASE GetPhase() { return phase; };
	protected:
		const FMOP* param;
		EG_PHASE phase;
		SINT16 value;
		UINT16 count;
	};
	class CLFOControl {
	public:
		enum LFO_PHASE { LFO_DELAYING, LFO_INCREASING, LFO_RETAINING, LFO_NONE, };
		CLFOControl();
		~CLFOControl() {};
		UINT8 GetValue() { return UINT8(value); };
		UINT16 GetCount() { return count; };
		void SetParam(UINT8 dly, UINT8 rt);
		LFO_PHASE GetStatus() { return status; };
		void Start(UINT8 dly, UINT8 rate);
		void Stop();
		int Update();
	protected:
		UINT16 count;
		UINT16 rate;
		UINT16 value;
		UINT16 delay;
		LFO_PHASE status;
	};
	struct LFORESOURCE {
		LFORESOURCE();
		CMidiCh* parent;
		UINT32 used;
		UINT8 amrate;
		UINT8 amdepth;
		UINT8 pmrate;
		UINT8 pmdepth;
	};
	class CHATTR {
	protected:
		enum CHSTAT {
			EMPTY = 0, DISABLED = -1, ASSIGNED = 1, RUNNING = 2,
		} status;
		bool dva;
		CSoundDevice* device;
		CMidiCh* parent;
		FMVOICE	voice;
		UINT8	lastnote;
		SINT16	finefreq;
		FNUM	lastfnum;
	public:
		UINT8	volume;
		UINT8	express;
		UINT8	velocity;
		SINT8	panpot;
		UINT8	lastvel;
		UINT16	count;
		UINT32	noteon;
		CLFOControl chlfo;
		CLFOControl oplfo[4];
		UINT8	baseTL[4];
	public:
		CHATTR();
		int IsEnable() const { return (status!=DISABLED); };
		int IsAvailable() const { return (status==EMPTY); };
		int IsRunning() const { return (status==RUNNING); };
		int IsAssigned() const { return (status==ASSIGNED); };
		int GetStatus() const { return status; };
		int GetCount() const { return count; };
		bool IsAutoAssignable() const { return dva; };
		CMidiCh* GetParent() const { return parent; }
		FMVOICE* GetVoice() const { return (FMVOICE* const)&voice; };
		UINT32 GetVoiceID() const { return voice.ID; };
		BOOL SetVoice(FMVOICE* vc);
		void SetVoiceID(UINT32 vcid);
		void Init();
		void Assign(CMidiCh* parch);
		void Run();
		void Stop();
		void Release(int par);
		void Enable(int ena);
		void SetNoteFine(UINT8 note, SINT16 fine);
		void OutOfDVA() { dva = false; };
		UINT8 GetLastNote() const { return lastnote; }
		SINT16 GetLastFineFreq() const { return finefreq; };
		SINT16 GetChLFOValue();
		SINT32 GetNoteIndex(SINT16 offset = 0);
		const FNUM* GetLastFnumber() const { return &lastfnum; };
		void UpdateFnumber(FNUM* fnum);
		UINT8 GetEffectiveLevel();
	};
public:
	// Device status
	virtual void SetDevice(UINT8 devid) = 0;
	virtual UINT8 GetDevice() = 0;
	virtual UINT8 GetChipCount() = 0;
	virtual void EnableCh(UINT8 ch, UINT8 ena) = 0;
	virtual UINT8 AllocCh(CMidiCh* parent, FMVOICE* voice) = 0;
	virtual UINT8 Assign(UINT8 ch, CMidiCh* parent, FMVOICE* voice) = 0;
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) = 0;
	virtual void ReleaseCh(UINT8 ch) = 0;
	virtual UINT8 GetChs() = 0;
	virtual UINT8 GetAvailable() = 0;
	virtual UINT8 GetAssignable() = 0;
	virtual CPort* GetDevPort() = 0;
	virtual void Reset() = 0;
	virtual void EnableDevPM(UINT8 ch, UINT8 on) = 0;
	virtual void EnableDevAM(UINT8 ch, UINT8 on) = 0;
	virtual void SetDevPMDepth(UINT8 ch, UINT8 dep) = 0;
	virtual void SetDevAMDepth(UINT8 ch, UINT8 dep) = 0;
	virtual void SetDevPMRate(UINT8 ch, UINT8 rate) = 0;
	virtual void SetDevAMRate(UINT8 ch, UINT8 rate) = 0;

	// Physical accesses
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v=1) = 0;
	virtual UINT8 GetReg(UINT16 reg, UINT8 v) = 0;
	virtual void Flush() = 0;
	virtual void Init() = 0;

	// Note control interfaces (polymorph)
	virtual void NoteOn(UINT8 ch, UINT8 vel=127) = 0;
	virtual void NoteOff(UINT8 ch) = 0;

	// Ch Property settings
	virtual void SetSustain(UINT8 ch, UINT8 sus, int update=1) = 0;
	virtual void SetNoteFine(UINT8 ch, UINT8 note, SINT16 fine, int update=1) = 0;
	virtual void SetVoice(UINT8 ch, FMVOICE* voice, int update=1) = 0;
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1) = 0;
	virtual void SetVolume(UINT8 ch, UINT8 vol, int update=1) = 0;
	virtual void SetVelocity(UINT8 ch, UINT8 vel, int update=1) = 0;
	virtual void SetExpress(UINT8 ch, UINT8 exp, int update=1) = 0;
	virtual void SetPanpot(UINT8 ch, UINT8 pan, int update=1) = 0;
	virtual void SetMasterVolume(UINT8 vol, int update=1) = 0;
	virtual void SetLFOParam(FMVOICE* voice, int update=1) = 0;
	virtual void SetLFOMode(UINT8 ch, UINT8 mode, int update=1) = 0;
	virtual void OverrideAttribute(CHATTR* attr) = 0;

	//For Monitoring
	virtual UINT32 GetCurrentVoiceID(UINT8 ch) const = 0;
	virtual FMVOICE* GetCurrentVoice(UINT8 ch) const = 0;
	virtual UINT8 GetCurrentNote(UINT8 ch) const = 0;
	virtual FNUM GetCurrentFnum(UINT8 ch) const = 0;
	virtual void GetDescriptor(TCHAR* str, size_t len) = 0;

	//Callback
	virtual void PollingCallBack() = 0;
	virtual void TimerCallBack(UINT32 tick) = 0;
};

class CSoundDevice : public ISoundDevice {
	friend class CMultiDevice;
	friend class CSpanDevice;
	friend class CSplitPan;
	friend class CLinearPan;
	friend class CUnison;
	friend class CPSGBase;
	friend class CAdPcmBase;
	friend class CRhythmDevice;
protected:
	UINT8	device;
	UINT8	chs;	//
	UINT8	potency;	//
	UINT8	ops;
	UINT8	volume;
	UINT8	prior_ch;
	CPort*	port;
	CHATTR*	chattr;
	UINT8*	regbak;
	const UINT16* Fnum;
	int	NoteOffset;
	int MasterTune;

	//Updater
	virtual void UpdateSustain(UINT8 ch) = 0;
	virtual void UpdateVolExp(UINT8 ch) = 0;
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum=0) = 0;
	virtual void UpdateVoice(UINT8 ch) = 0;
	virtual void UpdatePanpot(UINT8 ch) = 0;
	virtual void UpdateOpLFO(UINT8 ch, UINT8 op);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon) = 0;
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev) = 0;
	virtual void UpdateFnumber(UINT8 ch, int update=1);
	//Internal utility
	virtual FNUM GetFnumber(UINT8 ch, SINT16 offset = 0);
public:
	CSoundDevice(UINT8 devid, UINT8 maxchs, int fmas, int devide, int offset = FNUM_OFFSET, FnumTableType ftype = FnumTableType::Fnumber, CPort* pt = 0, int regsize = 0);
	//CSoundDevice();
	~CSoundDevice();

	virtual void NoteOn(UINT8 ch, UINT8 vel=127);
	virtual void NoteOff(UINT8 ch);
	virtual void Damp(UINT8 ch) { NoteOff(ch); };

	// Device status
	virtual void SetDevice(UINT8 devid);
	virtual UINT8 GetDevice() { return device; };
	virtual UINT8 GetChipCount() { return 1; };
	virtual int GetLocation();
	virtual void EnableCh(UINT8 ch, UINT8 ena);
	virtual UINT8 AllocCh(CMidiCh* parent, FMVOICE* voice = 0);
	virtual UINT8 Assign(UINT8 ch, CMidiCh* parent, FMVOICE* voice);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void ReleaseCh(UINT8 ch);
	virtual UINT8 GetChs() { return chs; };
	virtual UINT8 GetAvailable();
	virtual UINT8 GetAssignable();
	virtual CPort* GetDevPort() { return port; };
	virtual void Reset();
	virtual void Init() = 0;
	virtual void SetPresetTone(UINT8 tn, FMVOICE* voice) {};
	virtual void UpdatePresetTone() {};

	// Physical accesses
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v=1);
	virtual UINT8 GetReg(UINT16 reg, UINT8 v=0);
	virtual void Flush() {};

	// Ch Property settings
	virtual void SetSustain(UINT8 ch, UINT8 sus, int update=1);
	virtual void SetNoteFine(UINT8 ch, UINT8 note, SINT16 fine, int update=1);
	virtual void SetVoice(UINT8 ch, FMVOICE* voice, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void SetVolume(UINT8 ch, UINT8 vol, int update=1);
	virtual void SetVelocity(UINT8 ch, UINT8 vel, int update=1);
	virtual void SetExpress(UINT8 ch, UINT8 exp, int update=1);
	virtual void SetPanpot(UINT8 ch, UINT8 pan, int update=1);
	virtual void SetMasterVolume(UINT8 vol, int update=1);
	virtual void SetLFOParam(FMVOICE* voice, int update=1) {};
	virtual void SetLFOMode(UINT8 ch, UINT8 mode, int update=1) {};

	virtual void EnableDevPM(UINT8 ch, UINT8 on) {};
	virtual void EnableDevAM(UINT8 ch, UINT8 on) {};
	virtual void SetDevPMDepth(UINT8 ch, UINT8 dep) {};
	virtual void SetDevAMDepth(UINT8 ch, UINT8 dep) {};
	virtual void SetDevPMRate(UINT8 ch, UINT8 rate) {};
	virtual void SetDevAMRate(UINT8 ch, UINT8 rate) {};

	virtual void OverrideAttribute(CHATTR* attr);

	//For Monitoring
	virtual UINT32 GetCurrentVoiceID(UINT8 ch) const;
	virtual FMVOICE* GetCurrentVoice(UINT8 ch) const;
	virtual UINT8 GetCurrentNote(UINT8 ch) const;
	virtual FNUM GetCurrentFnum(UINT8 ch) const;
	virtual void GetDescriptor(TCHAR* str, size_t len);
	virtual CHATTR* GetChAttribute(UINT8 ch) const;

	//Callback
	virtual void PollingCallBack();
	virtual void TimerCallBack(UINT32 tick);
};
typedef CSoundDevice* CSoundDevicePtr;

class CRhythmDevice : public CSoundDevice
{
public:
	CRhythmDevice(CSoundDevice* parent, UINT8 devid, UINT8 maxch);
protected:
	CSoundDevice* pParent;
	//Updater
	virtual void UpdateSustain(UINT8 ch) {};
	virtual void UpdateVolExp(UINT8 ch) = 0;
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum = 0) = 0;
	virtual void UpdateVoice(UINT8 ch) = 0;
	virtual void UpdatePanpot(UINT8 ch) = 0;
	virtual void UpdateOpLFO(UINT8 ch, UINT8 op) {};
	virtual void UpdateKey(UINT8 ch, UINT8 keyon) = 0;
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev) {};
	virtual void UpdateFnumber(UINT8 ch, int update = 1) {};
public:
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) = 0;
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v = 1) { if (pParent) { pParent->SetReg(reg, data, v); } };
	virtual UINT8 GetReg(UINT16 reg, UINT8 v = 0) { return pParent ? pParent->GetReg(reg, v) : 0; };
	virtual void Flush() { if (pParent) { pParent->Flush(); } };
};

#endif
