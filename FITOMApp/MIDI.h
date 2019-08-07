#ifndef __MIDI_H__
#define __MIDI_H__

#define	MAX_SYSEX	8192
#define	RPN_ENTRY	1
#define	NRPN_ENTRY	2
#define	RPN_PITCHBEND	0x0000
#define	RPN_FINETUNE	0x0001
#define	RPN_COASETUNE	0x0002
#define	RPN_TUNEPROG	0x0003
#define	RPN_TUNEBANK	0x0004
#define	RPN_MOD_DEPTH	0x0005

#define NRPN_PMRATE		1
#define NRPN_PMWAVE		2
#define NRPN_AMRATE		4
#define NRPN_AMWAVE		5

#define NRPN_PHYCH		0x3001
#define NRPN_DIRADR		0x3002
#define NRPN_DIRDAT		0x3003

#define NRPN_DRUM_PITCH	0x1800
#define NRPN_DRUM_LEVEL	0x1A00
#define NRPN_DRUM_PAN	0x1C00

#define NRPN_DRUM_VOFF	0x2000
#define NRPN_DRUM_POFF	0x2100

#define	NRPN_GPCTRL1		0x3010
#define	NRPN_GPCTRL2		0x3011
#define	NRPN_GPCTRL3		0x3012
#define	NRPN_GPCTRL4		0x3013
#define	NRPN_GPCTRL5		0x3014
#define	NRPN_GPCTRL6		0x3015
#define	NRPN_GPCTRL7		0x3016
#define	NRPN_GPCTRL8		0x3017

#define	RPN_NULL		16383

#define MID_KAWAI	0x40
#define MID_ROLAND	0x41
#define MID_KORG	0x42
#define MID_YAMAHA	0x43
#define MID_UNRT	0x7e
#define MID_URT		0x7f

#include "FITOMdefine.h"
#include "SoundDev.h"

class CFITOM;

class CMidiCh {
protected:
	CFITOM* Parent;

	uint16_t	NRPNReg;
	uint16_t	RPNReg;
	uint8_t	DataLSB;
	uint8_t	EntryMode;

public:
	CMidiCh(uint8_t ch, CFITOM* parent);
	~CMidiCh() {};
	void SetParent(CFITOM* parent) { Parent = parent; };

	//Channel message
	virtual void Assign(CSoundDevice* dev, uint8_t pol) {};
	virtual void ProgChange(uint8_t prog) {};
	virtual void SetPitchBend(uint16_t pt) {};
	virtual void SetPortamento(uint8_t on) {};
	virtual void SetLegato(uint8_t leg) {};
	virtual void SetSustain(uint8_t sus) {};
	virtual void NoteOn(uint8_t note, uint8_t vel) {};
	virtual void NoteOff(uint8_t note) {};
	virtual void ResetAllCtrl();
	virtual void AllNoteOff();
	virtual void SetPoly(uint8_t poly) {};
	virtual void SetMono(uint8_t mono) {};

	//Control Change
	virtual void BankSel(uint16_t bank) {};
	virtual void BankSelMSB(uint8_t bsm) {};
	virtual void BankSelLSB(uint8_t bsl) {};
	virtual void SetVolume(uint8_t vol) {};
	virtual void SetExpress(uint8_t exp) {};
	virtual void SetModulation(uint8_t dep) {};
	virtual void SetFootCtrl(uint8_t dep) {};
	virtual void SetBreathCtrl(uint8_t dep) {};
	virtual void SetPanpot(uint8_t pan) {};
	virtual void SetPortTime(uint8_t port) {};
	virtual void SetSourceNote(uint8_t note) {};
	virtual void SetForceDamp(uint8_t rel) {};
	virtual void SetSostenuto(uint8_t sos) {};

	//RPN/NRPN
	virtual void SetRPNMSB(uint8_t rpn);
	virtual void SetRPNLSB(uint8_t rpn);
	virtual void SetNRPNMSB(uint8_t nrpn);
	virtual void SetNRPNLSB(uint8_t nrpn);
	virtual void SetRPNRegister(uint16_t reg, uint16_t data) {};
	virtual void SetNRPNRegister(uint16_t reg, uint16_t data) {};
	virtual void DataEntry(uint8_t data);
	virtual void DataEntryLSB(uint8_t data) { DataLSB = data; };
	virtual void DataIncrement(int16_t data);
	virtual void DataDecrement(int16_t data);
	virtual void SetPMRate(uint8_t rate) {};
	virtual void SetAMRate(uint8_t rate) {};
	virtual void SetFineTune(uint16_t tune) {};
	virtual void SetBendRange(uint8_t range) {};
	virtual void SetPhyCh(uint8_t ch) {};

	virtual void SetRegAddr(uint16_t addr) {};
	virtual void SetVoiceAddr(uint8_t addr) {};
	virtual void SetRegData(uint8_t data) {};
	virtual void SetVoiceData(uint8_t data) {};
	virtual void SetVoiceData(FMVOICE* data) {};

	//Channel Monitor
	virtual uint32_t GetDeviceID() const { return 0xff; };
	virtual CSoundDevice* GetDevice() const { return NULL; };
	virtual uint8_t GetLastNote() const { return 0xff; };
	virtual uint32_t GetLastProg() const { return 0xffffffL; };
	virtual uint8_t GetProgramNo() const { return 0xff; };
	virtual uint16_t GetBankNo() const { return 0xff; };
	virtual uint8_t GetTrackVolume() const { return 0; };
	virtual uint8_t GetPoly() const { return 0; };
	virtual uint8_t GetPanpot() const { return 64; };
	virtual uint8_t GetExpress() const { return 0; };
	virtual uint8_t GetModulation() const { return 0; };
	virtual uint8_t GetFootCtrl() const { return 0; };
	virtual uint8_t GetPMRate() const { return 0; };
	virtual uint8_t GetAMRate() const { return 0; };
	virtual uint8_t GetPortamento() const { return 0; };
	virtual uint8_t GetPortTime() const { return 0; };
	virtual uint8_t GetLegato() const { return 0; };
	virtual uint8_t GetSustain() const { return 0; };
	virtual uint8_t GetSostenuto() const { return 0; }
	virtual uint8_t GetForceDamp() const { return 0; };
	virtual uint16_t GetPitchBend() const { return 0; };
	virtual uint8_t GetBendRange() const { return 0; };
	virtual uint16_t GetFineTune() const { return 8192; };
	virtual uint8_t GetPhyCh() const { return 127; };
	virtual uint16_t GetNRPNRegister(uint16_t reg) { return 0; };
	virtual uint16_t GetRPNRegister(uint16_t reg) { return 0; };

	virtual void MIDIClockCallBack(uint32_t tick) = 0;
	virtual void PollingCallBack() = 0;
	virtual void TimerCallBack(uint32_t tick) = 0;
	virtual int IsInst() { return FALSE; };
	virtual int IsRhythm() { return FALSE; };
	virtual int IsThru() { return FALSE; };
};

class CInstCh : public CMidiCh {
protected:
	uint8_t	timbres;
	FMVOICE voice;
	CSoundDevice* Device;
	struct CHHIST {
		uint8_t	ch;
		uint8_t	note;
		int16_t	fine;
		CSoundDevice* dev;
		//uint16_t	count;
	} Note[16];
	class CPortaCtrl {
	public:
		enum PORT_STATE { STOPPING, RUNNING, };
	protected:
		uint8_t start;
		uint8_t end;
		uint8_t current;
		uint8_t fine;
		uint8_t enable;
		uint8_t speed;
		uint32_t count;
		PORT_STATE status;
	public:
		CPortaCtrl();
		void Start(uint8_t dst);
		void SetSource(uint8_t note);
		void Stop();
		void Init();
		void Update();
		PORT_STATE GetStatus();
		int GetCount();
		void Enable(int flag);
		void SetSpeed(uint8_t sp);
		uint8_t GetCurrentNote() const { return current; };
		uint8_t GetCurrentFine() const { return fine; };
		uint8_t GetSpeed() const { return speed; };
		uint8_t IsEnable() const { return enable; };
	} Portamento;
	void Enter(uint8_t ch, uint8_t note, uint16_t id);
	void Leave(uint8_t idx);
	uint8_t FindHist(uint8_t ch, uint8_t note, uint16_t id);

	//Channel Status
	uint8_t	ProgramNo;
	uint16_t	PitchBend;
	//Control Change
	uint8_t	Volume;
	uint8_t	Panpot;
	uint8_t	BankSelL;
	uint8_t	BankSelM;
	uint8_t	Expression;
	uint8_t	PMDepth;
	uint8_t	AMDepth;
	uint8_t	VibRate;
	uint8_t	VibDepth;
	uint8_t	VibDelay;
	uint8_t	Resonance;
	uint8_t	Brightness;
	uint8_t	AttackTime;
	uint8_t	DecayTime;
	uint8_t	ReleaseTime;
	//RPN
	uint8_t	BendRange;
	uint16_t	Tuning;
	//NRPN
	uint8_t	PMRate;
	uint8_t	AMRate;
	uint8_t	PhyCh;
	uint16_t	DirAddr;
	uint16_t	DirData;

	//Internal
	uint8_t	Ch;
	uint8_t	Poly;
	uint8_t	Mono;

	uint8_t	Legato;
	uint8_t	Sustain;
	uint8_t	ForceDamp;
	uint8_t	Sostenuto;

	struct AutoSeq {
		uint8_t mode;
		uint8_t steps;
		uint8_t tick;
		uint8_t gate;
		uint8_t note[16];
	} seqparam;
	//Device Control
	uint16_t	RegAddr;
	uint8_t	VoiceAddr;
	void UpdateFineTune();
	void SetPresetTone(uint16_t tn, uint16_t data);
public:
	CInstCh(uint8_t ch, CFITOM* parent);
	~CInstCh() {};
	virtual void Assign(CSoundDevice* dev, uint8_t pol);
	virtual void ProgChange(uint8_t prog);
	virtual void SetVolume(uint8_t vol);
	virtual void SetExpress(uint8_t exp);
	virtual void SetPitchBend(uint16_t pt);
	virtual void SetPortamento(uint8_t on);
	virtual void SetPortTime(uint8_t pt);
	virtual void SetSourceNote(uint8_t note);
	virtual void SetFineTune(uint16_t tune);
	virtual void SetModulation(uint8_t dep);
	virtual void SetFootCtrl(uint8_t dep);
	virtual void SetBreathCtrl(uint8_t dep);
	virtual void SetPMRate(uint8_t rate);
	virtual void SetAMRate(uint8_t rate);
	virtual void SetPanpot(uint8_t pan);
	virtual void SetRPNRegister(uint16_t reg, uint16_t data);
	virtual void SetNRPNRegister(uint16_t reg, uint16_t data);
	virtual void SetPoly(uint8_t poly);
	virtual void SetMono(uint8_t mono);
	virtual void SetLegato(uint8_t leg);
	virtual void SetSustain(uint8_t sus);
	virtual void SetForceDamp(uint8_t rel);
	virtual void BankSel(uint16_t bank);
	virtual void BankSelMSB(uint8_t bsm);
	virtual void BankSelLSB(uint8_t bsl);
	virtual void SetBendRange(uint8_t range);
	virtual void NoteOn(uint8_t note, uint8_t vel);
	virtual void NoteOff(uint8_t note);
	virtual void ResetAllCtrl();
	virtual void AllNoteOff();
	virtual void SetPhyCh(uint8_t ch);

	virtual void SetRegAddr(uint8_t addr);
	virtual void SetVoiceAddr(uint8_t addr) { VoiceAddr = addr; };
	virtual void SetRegData(uint8_t data);
	virtual void SetVoiceData(uint8_t data);
	virtual void SetVoiceData(FMVOICE* data);

	//Monitor
	virtual uint32_t GetDeviceID() const;
	virtual CSoundDevice* GetDevice() const { return Device; };
	virtual uint8_t GetLastNote() const;
	virtual uint32_t GetLastProg() const;
	virtual uint8_t GetProgramNo() const { return ProgramNo; };
	virtual uint16_t GetBankNo() const { return BankSelL; };
	virtual uint8_t GetTrackVolume() const { return Volume; };
	virtual uint8_t GetPoly() const { return Poly; };
	virtual uint8_t GetMono() const { return Mono; };
	virtual uint8_t GetPanpot() const { return Panpot; };
	virtual uint8_t GetExpress() const { return Expression; };
	virtual uint8_t GetModulation() const { return PMDepth; };
	virtual uint8_t GetFootCtrl() const { return AMDepth; };
	virtual uint8_t GetPMRate() const { return PMRate; };
	virtual uint8_t GetAMRate() const { return AMRate; };
	virtual uint8_t GetPortamento() const { return Portamento.IsEnable(); };
	virtual uint8_t GetPortTime() const { return Portamento.GetSpeed(); };
	virtual uint8_t GetLegato() const { return Legato; };
	virtual uint8_t GetSustain() const { return Sustain; };
	virtual uint8_t GetSostenuto() const { return Sostenuto; };
	virtual uint8_t GetForceDamp() const { return ForceDamp; };
	virtual uint16_t GetPitchBend() const { return PitchBend; };
	virtual uint16_t GetFineTune() const { return Tuning; };
	virtual uint8_t GetBendRange() const { return BendRange; };
	virtual uint8_t GetPhyCh() const { return PhyCh; };
	virtual uint16_t GetRegAddr() const { return DirAddr; };
	virtual uint8_t GetRegData() const;
	virtual uint16_t GetNRPNRegister(uint16_t reg);
	virtual uint16_t GetRPNRegister(uint16_t reg);

	virtual int IsInst() { return TRUE; }

	virtual void MIDIClockCallBack(uint32_t tick);
	virtual void PollingCallBack() {};
	virtual void TimerCallBack(uint32_t tick) ;
};

class CRhythmCh : public CMidiCh {
protected:
	uint8_t LastNote;
	struct NOTEHIST {
		CSoundDevice* device;
		uint8_t ch;
		uint8_t note;
		uint16_t count;
	} Note[128];
	struct InstantAdjust {
		int16_t pan;
		int16_t vel;
		int16_t pitch;
	} instant[128];
	uint8_t Volume;
	uint8_t ProgramNo;
	void InitInstant();
public:
	CRhythmCh(uint8_t ch, CFITOM* parent);
	~CRhythmCh() {};
	virtual uint32_t GetDeviceID() const { return RHYTHM_BANK; };
	virtual void ProgChange(uint8_t prog);
	virtual void SetVolume(uint8_t vol);
	virtual void NoteOn(uint8_t note, uint8_t vel);
	virtual void NoteOff(uint8_t note);
	virtual uint8_t GetLastNote() const { return LastNote; };
	virtual uint32_t GetLastProg() const { return 0; };
	virtual uint8_t GetProgramNo() const { return ProgramNo; };
	virtual uint16_t GetBankNo() const { return 0; };
	virtual uint8_t GetTrackVolume() const { return Volume; };
	virtual void PollingCallBack() {};
	virtual void MIDIClockCallBack(uint32_t tick) {};
	virtual void TimerCallBack(uint32_t tick);
	virtual int IsRhythm() { return TRUE; };
	virtual void AllNoteOff();
	virtual void ResetAllCtrl();
	virtual void SetRPNRegister(uint16_t reg, uint16_t data);
	virtual void SetNRPNRegister(uint16_t reg, uint16_t data);
	virtual uint16_t GetNRPNRegister(uint16_t reg);
	virtual uint16_t GetRPNRegister(uint16_t reg);
	void NoteOn(uint8_t note, uint8_t vel, DRUMMAP* dm);
	void NoteOff(CSoundDevice* device, uint8_t ch);
};

class CMidiIn;

class CMidiInst {
protected:
	CMidiCh* ch[16];
	CMidiIn* Port;
	CFITOM* Parent;
	uint16_t currentstatus;
	uint8_t SysExBuf[MAX_SYSEX];
	uint16_t SysExPt;
	uint8_t MsgBuf[4];
	uint16_t MsgPt;
	enum CONDITION { COND_READY=0, COND_WAIT1=1, COND_WAIT2=2, COND_SYSEX=15, } cond;
	uint16_t ProcMsg(uint8_t msg);
	uint16_t SysEx();
	uint16_t Status3();
	uint16_t Status2();
	uint16_t Control();
	uint16_t SysMsg();
	uint16_t ChannelMap();
	BOOL clockenable;
	volatile BOOL bMidiProc;
	volatile BOOL bTimerProc;
public:
	CMidiInst(CMidiIn* port, CFITOM* parent, BOOL clkena);
	~CMidiInst();
	CMidiCh* GetMidiCh(uint8_t nch);
	void SetParent(CFITOM* parent);

	void AssignInst(uint8_t nch, CSoundDevice* dev, uint8_t pol);
	void AssignRhythm(uint8_t nch);
	void AssignThru(uint8_t nch, uint8_t och, uint8_t opt);
	void Release(uint8_t nch);
	int PollingCallBack();
	int InterruptCallBack(BYTE* buf, size_t length);
	void TimerCallBack(uint32_t tick);
	void MIDIClockCallBack(uint32_t tick);
	uint16_t SysExYamaha(BYTE* buf, size_t length);
	uint16_t SysExRoland(BYTE* buf, size_t length);
	uint16_t SysExUNRT(BYTE* buf, size_t length);
	uint16_t SysExURT(BYTE* buf, size_t length);
	const uint16_t GetCurrentStatus() { return currentstatus; };
};

class CThruCh : public CMidiCh {
protected:
	//Channel Status
	uint8_t	LastNote;
	uint8_t	ProgramNo;
	uint16_t	PitchBend;
	//Control Change
	uint8_t	Volume;
	uint8_t	Panpot;
	uint8_t	BankSelectMSB;
	uint8_t	BankSelectLSB;
	uint8_t	Expression;
	uint8_t	PMDepth;
	uint8_t	AMDepth;

	//RPN
	uint8_t	BendRange;
	uint16_t	Tuning;
	//NRPN
	uint8_t	PMRate;
	uint8_t	AMRate;

	//Internal
	uint8_t	OutPort;
	uint8_t	OutCh;
	uint8_t	Poly;
	uint8_t	Mono;
	uint8_t	Port;
	uint8_t	PortTime;
	uint8_t	Legato;
	uint8_t	Sustain;

public:
	CThruCh(uint8_t ch, uint8_t och, uint8_t opt, CFITOM* parent);
	~CThruCh() {};
	virtual void ProgChange(uint8_t prog) { ProgramNo = prog; };
	virtual void SetVolume(uint8_t vol) { Volume = vol; };
	virtual void SetExpress(uint8_t exp) { Expression = exp; };
	virtual void SetPitchBend(uint16_t pt) { PitchBend = pt; };
	virtual void SetPortamento(uint8_t on) { Port = on; };
	virtual void SetPortTime(uint8_t pt) { PortTime = pt; };
	virtual void SetFineTune(uint16_t tune) { Tuning = tune; };
	virtual void SetModulation(uint8_t dep) { PMDepth = dep; };
	virtual void SetFootCtrl(uint8_t dep) { AMDepth = dep; };
	virtual void SetBreathCtrl(uint8_t dep) {};
	virtual void SetPMRate(uint8_t rate) { PMRate = rate; };
	virtual void SetAMRate(uint8_t rate) { AMRate = rate; };
	virtual void SetPanpot(uint8_t pan) { Panpot = pan; };
	virtual void DataEntry(uint8_t data) {};
	virtual void SetPoly(uint8_t poly) { Poly = poly; Mono = 0; };
	virtual void SetMono(uint8_t mono) { Poly = 0; Mono = mono; };
	virtual void SetLegato(uint8_t leg) { Legato = leg; };
	virtual void SetSustain(uint8_t sus) { Sustain = sus; };
	virtual void BankSel(uint16_t bank) { BankSelectMSB = (bank >> 7) & 0x7f; BankSelectLSB = bank & 0x7f; };
	virtual void BankSelMSB(uint8_t bsm) { BankSelectMSB = bsm; };
	virtual void BankSelLSB(uint8_t bsl) { BankSelectLSB = bsl; };
	virtual void SetBendRange(uint8_t range) { BendRange = range; };
	virtual void NoteOn(uint8_t note, uint8_t vel) { LastNote = note; };
	virtual void NoteOff(uint8_t note) { LastNote = 0xff; };
	virtual void AllNoteOff() { LastNote = 0xff; };

	//Monitor
	virtual uint32_t GetDeviceID() const { return 0xff; };
	virtual uint8_t GetLastNote() const { return LastNote; };
	virtual uint32_t GetLastProg() const { return ProgramNo; };
	virtual uint8_t GetProgramNo() const { return ProgramNo; };
	virtual uint16_t GetBankNo() const { return BankSelectLSB; };
	virtual uint8_t GetTrackVolume() const { return Volume; };
	virtual uint8_t GetPoly() const { return Poly; };
	virtual uint8_t GetMono() const { return Mono; };
	virtual uint8_t GetPanpot() const { return Panpot; };
	virtual uint8_t GetExpress() const { return Expression; };
	virtual uint8_t GetModulation() const { return PMDepth; };
	virtual uint8_t GetFootCtrl() const { return AMDepth; };
	virtual uint8_t GetPMRate() const { return PMRate; };
	virtual uint8_t GetAMRate() const { return AMRate; };
	virtual uint8_t GetPortamento() const { return Port; };
	virtual uint8_t GetPortTime() const { return PortTime; };
	virtual uint8_t GetLegato() const { return Legato; };
	virtual uint8_t GetSustain() const { return Sustain; };
	virtual uint16_t GetPitchBend() const { return PitchBend; };
	virtual uint8_t GetBendRange() const { return BendRange; };

	virtual int IsThru() { return TRUE; }
	uint8_t GetOutPort() { return OutPort; };
	uint8_t GetOutCh() { return OutCh; };

	virtual void PollingCallBack() {};
	virtual void TimerCallBack(uint32_t tick) {};
};

#endif
