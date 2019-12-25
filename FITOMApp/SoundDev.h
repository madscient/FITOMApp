#ifndef __FMCHIP_H__
#define __FMCHIP_H__

#include "Port.h"
#include "Fnum.h"

#define FNUM_OFFSET	(-576)	//Fnum算出基点(440Hz)とノートNo基点(60=O4C)との差分(100/64セント単位)

#pragma pack(1)

struct FMOP {
	//EG parameter
	uint8_t	AR;  // 00:Attack Rate: 4bit OPL / 5bit OPN/OPM
	uint8_t	DR;  // 01:Decay Rate: 4bit OPL / 5bit OPN/OPM
	uint8_t	SL;  // 02:Sustain Level: 4bit OPL/OPN/OPM
	uint8_t	SR;  // 03:Sustain Rate: 4bit OPL / 5bit OPN/OPM
	uint8_t	RR;  // 04:Release Rate: 4bit OPL/OPN/OPM
	uint8_t	SRR; // 05:Sustain release rate 4bit
	uint8_t	TL;  // 06:Total Level: 6bit OPL / 7bit OPN/OPM
	uint8_t	EGR; // 07:EG bias 2bit | Reverbation 3 bit OPZ
	uint8_t	KS;  // 08:Key Scale Level: 2bit OPL / none OPN/OPM | Key Scale Rate: 1bit OPL / 2bit OPN/OPM
	uint8_t	WS;  // 09:Wave Select: 1bit OPLL / 2bit OPL2 / 3bit OPL3 / none OPN/OPM / 3bit OPZ | SSGEG
	//LFO parameter
	uint8_t	AVF; // 10:AM enable: 1bit OPL/OPNA/OPM | Vibrato: 1bit OPL/OPNA | Oscilator fixed freq (OPZ)
	uint8_t	SLF; // 11:Soft LFO frequency 0-15
	uint8_t	SLW; // 12:Soft LFO waveform 0:up saw/1:square/2:delta/3:s&h/4:down saw1/5:delta1/6:sin
	uint8_t	SLD; // 13:Soft LFO depth: 0 to 63, 64-127 for -64 to -1
	uint8_t	SLY; // 14:Soft LFO delay: 0-127 in 20ms unit
	uint8_t	SLR; // 15:Soft LFO rate: 0-127
	//Osc frequency
	uint8_t	MUL; // 16:Multiple: 4bit OPL/OPN/OPM
	uint8_t	DT1; // 17:Detune1: none OPL / 4bit OPN/OPM
	uint8_t	DT2; // 18:Detune2: none OPL/OPN / 3bit OPM
	uint8_t	DT3; // 19:fine frequency: 4bit OPZ ratio mode
	//Velocity scale
	uint8_t VSP; // 20:Velocity scale level 2bit (pattern No.) | Velocity sensitivity rate 2bit
	uint8_t VLT; // 21:Velocity level threshold 7bit
	uint8_t VRT; // 22:Velocity rate threshold 7bit
	uint8_t PAD; // 23:padding
};

struct FMVOICE {
	FMVOICE() : ID(0xffffffffUL) { name[0] = '\0'; };
	uint32_t	ID;  //00:ID
	char	name[16];//04:name
	uint8_t	FB;  // 20:Feedback: 3bit OPL/OPN/OPM
	uint8_t	AL;  // 21:Algorythm: 1bit OPL / 4bit OPL3 / 3bit OPN/OPM/MA3
	uint8_t	LDM; // 22:LFO Depth MSB: 0 to 8191, 8192-16383 for -8192 to -1
	uint8_t	LDL; // 23:LFO Depth LSB: 0 to 8191, 8192-16383 for -8192 to -1
	uint8_t	LFO; // 24:LFO frequency
	uint8_t	LWF; // 25:LFO Wave form 0:up saw/1:square/2:delta/3:s&h/4:down saw1/5:delta1/6:sin
	uint8_t	LFD; // 26:LFO delay: 0-127 in 20ms unit
	uint8_t	LFR; // 27:LFO rate: 0-127
	uint8_t	APS; // 28:AM Sensitivity: none OPL / 2bit OPM | PM Sensitivity: none OPL / 3bit OPM
	uint8_t	NFQ; // 29:Noise frequency: 5bit OPM/OPP/OPZ/SSG | H-LFO freq: 2bit MA3
	uint8_t PD0; // 30: padding
	uint8_t PD1; // 31: padding
	FMOP	op[4];
};

#pragma pack()

class CMidiCh;
extern int8_t GetLFOWave(uint8_t waveform, uint8_t speed, uint16_t phase);
extern uint8_t CalcVolExpVel(int vol, int exp, int vel);
extern uint8_t CalcEffectiveLevel(uint8_t vev, uint8_t tl);
extern uint8_t CalcLinearLevel(uint8_t vev, uint8_t tl);
extern uint8_t Linear2dB(uint8_t evol, int range, int step, int bw);

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
		FNUM(uint8_t bl, uint16_t fn);
		uint8_t	block;	// Block number/Key code(OPM)
		uint16_t	fnum;	// F-number/Key fractal(OPM)
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
		uint8_t GetValue() { return uint8_t(value); };
		EG_PHASE GetPhase() { return phase; };
	protected:
		const FMOP* param;
		EG_PHASE phase;
		int16_t value;
		uint16_t count;
	};
	class CLFOControl {
	public:
		enum LFO_PHASE { LFO_DELAYING, LFO_INCREASING, LFO_RETAINING, LFO_NONE, };
		CLFOControl();
		~CLFOControl() {};
		uint8_t GetValue() { return uint8_t(value); };
		uint16_t GetCount() { return count; };
		void SetParam(uint8_t dly, uint8_t rt);
		LFO_PHASE GetStatus() { return status; };
		void Start(uint8_t dly, uint8_t rate);
		void Stop();
		int Update();
	protected:
		uint16_t count;
		uint16_t rate;
		uint16_t value;
		uint16_t delay;
		LFO_PHASE status;
	};
	struct LFORESOURCE {
		LFORESOURCE();
		CMidiCh* parent;
		uint32_t used;
		uint8_t amrate;
		uint8_t amdepth;
		uint8_t pmrate;
		uint8_t pmdepth;
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
		uint8_t	lastnote;
		int16_t	finefreq;
		FNUM	lastfnum;
	public:
		//uint8_t	volume;
		//uint8_t	express;
		uint8_t	velocity;
		//int8_t	panpot;
		uint8_t	lastvel;
		//int8_t	vibrate;
		//int8_t	vibdepth;
		//int8_t	vibdelay;
		//int8_t	resonance;
		//int8_t	releasetime;
		//int8_t	attacktime;
		//int8_t	decaytime;
		//int8_t	brightness;
		uint16_t	count;
		uint32_t	noteon;
		CLFOControl chlfo;
		CLFOControl oplfo[4];
		uint8_t	baseTL[4];
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
		uint32_t GetVoiceID() const { return voice.ID; };
		BOOL SetVoice(FMVOICE* vc);
		void SetVoiceID(uint32_t vcid);
		void Init();
		void Assign(CMidiCh* parch);
		void Run();
		void Stop();
		void Release(int par);
		void Enable(int ena);
		void SetNoteFine(uint8_t note, int16_t fine);
		void OutOfDVA() { dva = false; };
		uint8_t GetLastNote() const { return lastnote; }
		int16_t GetLastFineFreq() const { return finefreq; };
		int16_t GetChLFOValue();
		int32_t GetNoteIndex(int16_t offset = 0);
		const FNUM* GetLastFnumber() const { return &lastfnum; };
		void UpdateFnumber(FNUM* fnum);
		uint8_t GetEffectiveLevel();
		uint8_t GetVolume();
		uint8_t GetExpress();
		uint8_t GetPanpot();
	};
public:
	// Device status
	virtual void SetDevice(uint8_t devid) = 0;
	virtual uint8_t GetDevice() = 0;
	virtual uint8_t GetChipCount() = 0;
	virtual void EnableCh(uint8_t ch, uint8_t ena) = 0;
	virtual uint8_t AllocCh(CMidiCh* parent, FMVOICE* voice) = 0;
	virtual uint8_t Assign(uint8_t ch, CMidiCh* parent, FMVOICE* voice) = 0;
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) = 0;
	virtual void ReleaseCh(uint8_t ch) = 0;
	virtual uint8_t GetChs() = 0;
	virtual uint8_t GetAvailable() = 0;
	virtual uint8_t GetAssignable() = 0;
	virtual CPort* GetDevPort() = 0;
	virtual void Reset() = 0;
	virtual void EnableDevPM(uint8_t ch, uint8_t on) = 0;
	virtual void EnableDevAM(uint8_t ch, uint8_t on) = 0;
	virtual void SetDevPMDepth(uint8_t ch, uint8_t dep) = 0;
	virtual void SetDevAMDepth(uint8_t ch, uint8_t dep) = 0;
	virtual void SetDevPMRate(uint8_t ch, uint8_t rate) = 0;
	virtual void SetDevAMRate(uint8_t ch, uint8_t rate) = 0;

	// Physical accesses
	virtual void SetReg(uint16_t reg, uint8_t data, uint8_t v=1) = 0;
	virtual uint8_t GetReg(uint16_t reg, uint8_t v) = 0;
	virtual void Flush() = 0;
	virtual void Init() = 0;

	// Note control interfaces (polymorph)
	virtual void NoteOn(uint8_t ch, uint8_t vel=127) = 0;
	virtual void NoteOff(uint8_t ch) = 0;

	// Ch Property settings
	virtual void SetSustain(uint8_t ch, uint8_t sus, int update=1) = 0;
	virtual void SetNoteFine(uint8_t ch, uint8_t note, int16_t fine, int update=1) = 0;
	virtual void SetVoice(uint8_t ch, FMVOICE* voice, int update=1) = 0;
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1) = 0;
	virtual void SetVolume(uint8_t ch, uint8_t vol, int update=1) = 0;
	virtual void SetVelocity(uint8_t ch, uint8_t vel, int update=1) = 0;
	virtual void SetExpress(uint8_t ch, uint8_t exp, int update=1) = 0;
	virtual void SetPanpot(uint8_t ch, uint8_t pan, int update=1) = 0;
	virtual void SetMasterVolume(uint8_t vol, int update=1) = 0;
	virtual void SetLFOParam(FMVOICE* voice, int update=1) = 0;
	virtual void SetLFOMode(uint8_t ch, uint8_t mode, int update=1) = 0;
	virtual void OverrideAttribute(CHATTR* attr) = 0;

	//For Monitoring
	virtual uint32_t GetCurrentVoiceID(uint8_t ch) const = 0;
	virtual FMVOICE* GetCurrentVoice(uint8_t ch) const = 0;
	virtual uint8_t GetCurrentNote(uint8_t ch) const = 0;
	virtual FNUM GetCurrentFnum(uint8_t ch) const = 0;
	virtual void GetDescriptor(TCHAR* str, size_t len) = 0;

	//Callback
	virtual void PollingCallBack() = 0;
	virtual void TimerCallBack(uint32_t tick) = 0;
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
	uint8_t	device;
	uint8_t	chs;	//
	uint8_t	potency;	//
	uint8_t	ops;
	uint8_t	volume;
	uint8_t	prior_ch;
	CPort*	port;
	CHATTR*	chattr;
	uint8_t*	regbak;
	const uint16_t* Fnum;
	int	NoteOffset;
	int MasterTune;

	//Updater
	virtual void UpdateSustain(uint8_t ch) = 0;
	virtual void UpdateVolExp(uint8_t ch) = 0;
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum=0) = 0;
	virtual void UpdateVoice(uint8_t ch) = 0;
	virtual void UpdatePanpot(uint8_t ch) = 0;
	virtual void UpdateOpLFO(uint8_t ch, uint8_t op);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon) = 0;
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev) = 0;
	virtual void UpdateFnumber(uint8_t ch, int update=1);
	//Internal utility
	virtual FNUM GetFnumber(uint8_t ch, int16_t offset = 0);
public:
	CSoundDevice(uint8_t devid, uint8_t maxchs, int fmas, int devide, int offset = FNUM_OFFSET, FnumTableType ftype = FnumTableType::Fnumber, CPort* pt = 0, int regsize = 0);
	//CSoundDevice();
	~CSoundDevice();

	virtual void NoteOn(uint8_t ch, uint8_t vel=127);
	virtual void NoteOff(uint8_t ch);
	virtual void Damp(uint8_t ch) { NoteOff(ch); };

	// Device status
	virtual void SetDevice(uint8_t devid);
	virtual uint8_t GetDevice() { return device; };
	virtual uint8_t GetChipCount() { return 1; };
	virtual int GetLocation();
	virtual void EnableCh(uint8_t ch, uint8_t ena);
	virtual uint8_t AllocCh(CMidiCh* parent, FMVOICE* voice = 0);
	virtual uint8_t Assign(uint8_t ch, CMidiCh* parent, FMVOICE* voice);
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void ReleaseCh(uint8_t ch);
	virtual uint8_t GetChs() { return chs; };
	virtual uint8_t GetAvailable();
	virtual uint8_t GetAssignable();
	virtual CPort* GetDevPort() { return port; };
	virtual void Reset();
	virtual void Init() = 0;
	virtual void SetPresetTone(uint8_t tn, FMVOICE* voice) {};
	virtual void UpdatePresetTone() {};

	// Physical accesses
	virtual void SetReg(uint16_t reg, uint8_t data, uint8_t v=1);
	virtual uint8_t GetReg(uint16_t reg, uint8_t v=0);
	virtual void Flush() {};

	// Ch Property settings
	virtual void SetSustain(uint8_t ch, uint8_t sus, int update=1);
	virtual void SetNoteFine(uint8_t ch, uint8_t note, int16_t fine, int update=1);
	virtual void SetVoice(uint8_t ch, FMVOICE* voice, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void SetVolume(uint8_t ch, uint8_t vol, int update=1);
	virtual void SetVelocity(uint8_t ch, uint8_t vel, int update=1);
	virtual void SetExpress(uint8_t ch, uint8_t exp, int update=1);
	virtual void SetPanpot(uint8_t ch, uint8_t pan, int update=1);
	virtual void SetMasterVolume(uint8_t vol, int update=1);
	virtual void SetLFOParam(FMVOICE* voice, int update=1) {};
	virtual void SetLFOMode(uint8_t ch, uint8_t mode, int update=1) {};

	virtual void EnableDevPM(uint8_t ch, uint8_t on) {};
	virtual void EnableDevAM(uint8_t ch, uint8_t on) {};
	virtual void SetDevPMDepth(uint8_t ch, uint8_t dep) {};
	virtual void SetDevAMDepth(uint8_t ch, uint8_t dep) {};
	virtual void SetDevPMRate(uint8_t ch, uint8_t rate) {};
	virtual void SetDevAMRate(uint8_t ch, uint8_t rate) {};

	virtual void OverrideAttribute(CHATTR* attr);

	//For Monitoring
	virtual uint32_t GetCurrentVoiceID(uint8_t ch) const;
	virtual FMVOICE* GetCurrentVoice(uint8_t ch) const;
	virtual uint8_t GetCurrentNote(uint8_t ch) const;
	virtual FNUM GetCurrentFnum(uint8_t ch) const;
	virtual void GetDescriptor(TCHAR* str, size_t len);
	virtual CHATTR* GetChAttribute(uint8_t ch) const;

	//Callback
	virtual void PollingCallBack();
	virtual void TimerCallBack(uint32_t tick);
};
typedef CSoundDevice* CSoundDevicePtr;

class CRhythmDevice : public CSoundDevice
{
public:
	CRhythmDevice(CSoundDevice* parent, uint8_t devid, uint8_t maxch);
	CRhythmDevice(CPort* parent, uint8_t devid, uint8_t maxch);
protected:
	CSoundDevice* pParent;
	//Updater
	virtual void UpdateSustain(uint8_t ch) {};
	virtual void UpdateVolExp(uint8_t ch) = 0;
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum = 0) = 0;
	virtual void UpdateVoice(uint8_t ch) = 0;
	virtual void UpdatePanpot(uint8_t ch) = 0;
	virtual void UpdateOpLFO(uint8_t ch, uint8_t op) {};
	virtual void UpdateKey(uint8_t ch, uint8_t keyon) = 0;
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev) {};
	virtual void UpdateFnumber(uint8_t ch, int update = 1) {};
public:
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) = 0;
};

#endif
