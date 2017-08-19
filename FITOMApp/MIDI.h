#ifndef __MIDI_H__
#define __MIDI_H__

#define	MAX_SYSEX	8192
#define	RPN_ENTRY	1
#define	NRPN_ENTRY	2
#define	RPN_PITCHBEND	0
#define	RPN_FINETUNE	1
#define	RPN_COASETUNE	2
#define	RPN_TUNEPROG	3
#define	RPN_TUNEBANK	4

#define NRPN_PMRATE		1
#define NRPN_PMWAVE		2
#define NRPN_AMRATE		4
#define NRPN_AMWAVE		5

#define NRPN_PRESET		0x1800
#define NRPN_PHYCH		0x2001
#define NRPN_DIRDEV		0x3001
#define NRPN_DIRADR		0x3002
#define NRPN_DIRDAT		0x3003

#define NRPN_DRUM_PITCH	0x1800
#define NRPN_DRUM_LEVEL	0x1A00
#define NRPN_DRUM_PAN	0x1C00

#define NRPN_DRUM_VOFF	0x2000
#define NRPN_DRUM_POFF	0x2100

#define	RPN_NULL		16383


#define MID_KAWAI	0x40
#define MID_ROLAND	0x41
#define MID_KORG	0x42
#define MID_YAMAHA	0x43
#define MID_UNRT	0x7e
#define MID_URT		0x7f

class CFITOM;

class CMidiCh {
protected:
	CFITOM* Parent;

	UINT16	NRPNReg;
	UINT16	RPNReg;
	UINT8	DataLSB;
	UINT8	EntryMode;

public:
	CMidiCh(UINT8 ch, CFITOM* parent);
	~CMidiCh() {};
	void SetParent(CFITOM* parent) { Parent = parent; };

	//Channel message
	virtual void Assign(CSoundDevice* dev, UINT8 pol) {};
	virtual void ProgChange(UINT8 prog) {};
	virtual void SetPitchBend(UINT16 pt) {};
	virtual void SetPortamento(UINT8 on) {};
	virtual void SetLegato(UINT8 leg) {};
	virtual void SetSustain(UINT8 sus) {};
	virtual void NoteOn(UINT8 note, UINT8 vel) {};
	virtual void NoteOff(UINT8 note) {};
	virtual void ResetAllCtrl();
	virtual void AllNoteOff();
	virtual void SetPoly(UINT8 poly) {};
	virtual void SetMono(UINT8 mono) {};

	//Control Change
	virtual void BankSel(UINT16 bank) {};
	virtual void BankSelMSB(UINT8 bsm) {};
	virtual void BankSelLSB(UINT8 bsl) {};
	virtual void SetVolume(UINT8 vol) {};
	virtual void SetExpress(UINT8 exp) {};
	virtual void SetModulation(UINT8 dep) {};
	virtual void SetFootCtrl(UINT8 dep) {};
	virtual void SetBreathCtrl(UINT8 dep) {};
	virtual void SetPanpot(UINT8 pan) {};
	virtual void SetPortTime(UINT8 port) {};
	virtual void SetSourceNote(UINT8 note) {};
	virtual void SetForceDamp(UINT8 rel) {};
	virtual void SetSostenuto(UINT8 sos) {};

	//RPN/NRPN
	virtual void SetRPNMSB(UINT8 rpn);
	virtual void SetRPNLSB(UINT8 rpn);
	virtual void SetNRPNMSB(UINT8 nrpn);
	virtual void SetNRPNLSB(UINT8 nrpn);
	virtual void SetRPNRegister(UINT16 reg, UINT16 data) {};
	virtual void SetNRPNRegister(UINT16 reg, UINT16 data) {};
	virtual void DataEntry(UINT8 data);
	virtual void DataEntryLSB(UINT8 data) { DataLSB = data; };
	virtual void DataIncrement(SINT16 data);
	virtual void DataDecrement(SINT16 data) { DataIncrement(-data); };
	virtual void SetPMRate(UINT8 rate) {};
	virtual void SetAMRate(UINT8 rate) {};
	virtual void SetFineTune(UINT16 tune) {};
	virtual void SetBendRange(UINT8 range) {};
	virtual void SetPhyCh(UINT8 ch) {};

	virtual void SetRegAddr(UINT16 addr) {};
	virtual void SetVoiceAddr(UINT8 addr) {};
	virtual void SetRegData(UINT8 data) {};
	virtual void SetVoiceData(UINT8 data) {};
	virtual void SetVoiceData(FMVOICE* data) {};

	//Channel Monitor
	virtual UINT32 GetDeviceID() const { return 0xff; };
	virtual CSoundDevice* GetDevice() const { return NULL; };
	virtual UINT8 GetLastNote() const { return 0xff; };
	virtual UINT32 GetLastProg() const { return 0xffffffL; };
	virtual UINT8 GetProgramNo() const { return 0xff; };
	virtual UINT16 GetBankNo() const { return 0xff; };
	virtual UINT8 GetTrackVolume() const { return 0; };
	virtual UINT8 GetPoly() const { return 0; };
	virtual UINT8 GetPanpot() const { return 64; };
	virtual UINT8 GetExpress() const { return 0; };
	virtual UINT8 GetModulation() const { return 0; };
	virtual UINT8 GetFootCtrl() const { return 0; };
	virtual UINT8 GetPMRate() const { return 0; };
	virtual UINT8 GetAMRate() const { return 0; };
	virtual UINT8 GetPortamento() const { return 0; };
	virtual UINT8 GetPortTime() const { return 0; };
	virtual UINT8 GetLegato() const { return 0; };
	virtual UINT8 GetSustain() const { return 0; };
	virtual UINT8 GetSostenuto() const { return 0; }
	virtual UINT8 GetForceDamp() const { return 0; };
	virtual UINT16 GetPitchBend() const { return 0; };
	virtual UINT8 GetBendRange() const { return 0; };
	virtual UINT16 GetFineTune() const { return 8192; };
	virtual UINT8 GetPhyCh() const { return 127; };
	virtual UINT16 GetNRPNRegister(UINT16 reg) { return 0; };
	virtual UINT16 GetRPNRegister(UINT16 reg) { return 0; };

	virtual void MIDIClockCallBack(UINT32 tick) = 0;
	virtual void PollingCallBack() = 0;
	virtual void TimerCallBack(UINT32 tick) = 0;
	virtual int IsInst() { return FALSE; };
	virtual int IsRhythm() { return FALSE; };
	virtual int IsThru() { return FALSE; };
};

class CInstCh : public CMidiCh {
protected:
	UINT8	timbres;
	FMVOICE voice;
	CSoundDevice* Device;
	struct CHHIST {
		UINT8	ch;
		UINT8	note;
		SINT16	fine;
		CSoundDevice* dev;
		//UINT16	count;
	} Note[16];
	class CPortaCtrl {
	public:
		enum PORT_STATE { STOPPING, RUNNING, };
	protected:
		UINT8 start;
		UINT8 end;
		UINT8 current;
		UINT8 fine;
		UINT8 enable;
		UINT8 speed;
		UINT32 count;
		PORT_STATE status;
	public:
		CPortaCtrl();
		void Start(UINT8 dst);
		void SetSource(UINT8 note);
		void Stop();
		void Init();
		void Update();
		PORT_STATE GetStatus();
		int GetCount();
		void Enable(int flag);
		void SetSpeed(UINT8 sp);
		UINT8 GetCurrentNote() const { return current; };
		UINT8 GetCurrentFine() const { return fine; };
		UINT8 GetSpeed() const { return speed; };
		UINT8 IsEnable() const { return enable; };
	} Portamento;
	void Enter(UINT8 ch, UINT8 note, UINT16 id);
	void Leave(UINT8 idx);
	UINT8 FindHist(UINT8 ch, UINT8 note, UINT16 id);

	//Channel Status
	UINT8	ProgramNo;
	UINT16	PitchBend;
	//Control Change
	UINT8	Volume;
	UINT8	Panpot;
	UINT8	BankSelL;
	UINT8	BankSelM;
	UINT8	Expression;
	UINT8	PMDepth;
	UINT8	AMDepth;

	//RPN
	UINT8	BendRange;
	UINT16	Tuning;
	//NRPN
	UINT8	PMRate;
	UINT8	AMRate;
	UINT8	PhyCh;
	UINT16	DirAddr;
	UINT16	DirData;

	//Internal
	UINT8	Ch;
	UINT8	Poly;
	UINT8	Mono;

	UINT8	Legato;
	UINT8	Sustain;
	UINT8	ForceDamp;
	UINT8	Sostenuto;

	struct AutoSeq {
		UINT8 mode;
		UINT8 steps;
		UINT8 tick;
		UINT8 gate;
		UINT8 note[16];
	} seqparam;
	//Device Control
	UINT16	RegAddr;
	UINT8	VoiceAddr;
	void UpdateFineTune();
	void SetPresetTone(UINT16 tn, UINT16 data);
public:
	CInstCh(UINT8 ch, CFITOM* parent);
	~CInstCh() {};
	virtual void Assign(CSoundDevice* dev, UINT8 pol);
	virtual void ProgChange(UINT8 prog);
	virtual void SetVolume(UINT8 vol);
	virtual void SetExpress(UINT8 exp);
	virtual void SetPitchBend(UINT16 pt);
	virtual void SetPortamento(UINT8 on);
	virtual void SetPortTime(UINT8 pt);
	virtual void SetSourceNote(UINT8 note);
	virtual void SetFineTune(UINT16 tune);
	virtual void SetModulation(UINT8 dep);
	virtual void SetFootCtrl(UINT8 dep);
	virtual void SetBreathCtrl(UINT8 dep);
	virtual void SetPMRate(UINT8 rate);
	virtual void SetAMRate(UINT8 rate);
	virtual void SetPanpot(UINT8 pan);
	virtual void SetRPNRegister(UINT16 reg, UINT16 data);
	virtual void SetNRPNRegister(UINT16 reg, UINT16 data);
	virtual void SetPoly(UINT8 poly);
	virtual void SetMono(UINT8 mono);
	virtual void SetLegato(UINT8 leg);
	virtual void SetSustain(UINT8 sus);
	virtual void SetForceDamp(UINT8 rel);
	virtual void BankSel(UINT16 bank);
	virtual void BankSelMSB(UINT8 bsm);
	virtual void BankSelLSB(UINT8 bsl);
	virtual void SetBendRange(UINT8 range);
	virtual void NoteOn(UINT8 note, UINT8 vel);
	virtual void NoteOff(UINT8 note);
	virtual void ResetAllCtrl();
	virtual void AllNoteOff();
	virtual void SetPhyCh(UINT8 ch);

	virtual void SetRegAddr(UINT8 addr);
	virtual void SetVoiceAddr(UINT8 addr) { VoiceAddr = addr; };
	virtual void SetRegData(UINT8 data);
	virtual void SetVoiceData(UINT8 data);
	virtual void SetVoiceData(FMVOICE* data);

	//Monitor
	virtual UINT32 GetDeviceID() const;
	virtual CSoundDevice* GetDevice() const { return Device; };
	virtual UINT8 GetLastNote() const;
	virtual UINT32 GetLastProg() const;
	virtual UINT8 GetProgramNo() const { return ProgramNo; };
	virtual UINT16 GetBankNo() const { return BankSelL; };
	virtual UINT8 GetTrackVolume() const { return Volume; };
	virtual UINT8 GetPoly() const { return Poly; };
	virtual UINT8 GetMono() const { return Mono; };
	virtual UINT8 GetPanpot() const { return Panpot; };
	virtual UINT8 GetExpress() const { return Expression; };
	virtual UINT8 GetModulation() const { return PMDepth; };
	virtual UINT8 GetFootCtrl() const { return AMDepth; };
	virtual UINT8 GetPMRate() const { return PMRate; };
	virtual UINT8 GetAMRate() const { return AMRate; };
	virtual UINT8 GetPortamento() const { return Portamento.IsEnable(); };
	virtual UINT8 GetPortTime() const { return Portamento.GetSpeed(); };
	virtual UINT8 GetLegato() const { return Legato; };
	virtual UINT8 GetSustain() const { return Sustain; };
	virtual UINT8 GetSostenuto() const { return Sostenuto; };
	virtual UINT8 GetForceDamp() const { return ForceDamp; };
	virtual UINT16 GetPitchBend() const { return PitchBend; };
	virtual UINT16 GetFineTune() const { return Tuning; };
	virtual UINT8 GetBendRange() const { return BendRange; };
	virtual UINT8 GetPhyCh() const { return PhyCh; };
	virtual UINT16 GetRegAddr() const { return DirAddr; };
	virtual UINT8 GetRegData() const;
	virtual UINT16 GetNRPNRegister(UINT16 reg);
	virtual UINT16 GetRPNRegister(UINT16 reg);

	virtual int IsInst() { return TRUE; }

	virtual void MIDIClockCallBack(UINT32 tick);
	virtual void PollingCallBack() {};
	virtual void TimerCallBack(UINT32 tick) ;
};

class CRhythmCh : public CMidiCh {
protected:
	UINT8 LastNote;
	struct NOTEHIST {
		CSoundDevice* device;
		UINT8 ch;
		UINT8 note;
		UINT16 count;
	} Note[128];
	struct InstantAdjust {
		SINT16 pan;
		SINT16 vel;
		SINT16 pitch;
	} instant[128];
	UINT8 Volume;
	UINT8 ProgramNo;
	void InitInstant();
public:
	CRhythmCh(UINT8 ch, CFITOM* parent);
	~CRhythmCh() {};
	virtual UINT32 GetDeviceID() const { return RHYTHM_BANK; };
	virtual void ProgChange(UINT8 prog);
	virtual void SetVolume(UINT8 vol);
	virtual void NoteOn(UINT8 note, UINT8 vel);
	virtual void NoteOff(UINT8 note);
	virtual UINT8 GetLastNote() const { return LastNote; };
	virtual UINT32 GetLastProg() const { return 0; };
	virtual UINT8 GetProgramNo() const { return ProgramNo; };
	virtual UINT16 GetBankNo() const { return 0; };
	virtual UINT8 GetTrackVolume() const { return Volume; };
	virtual void PollingCallBack() {};
	virtual void MIDIClockCallBack(UINT32 tick) {};
	virtual void TimerCallBack(UINT32 tick);
	virtual int IsRhythm() { return TRUE; };
	virtual void AllNoteOff();
	virtual void ResetAllCtrl();
	virtual void SetRPNRegister(UINT16 reg, UINT16 data);
	virtual void SetNRPNRegister(UINT16 reg, UINT16 data);
	virtual UINT16 GetNRPNRegister(UINT16 reg);
	virtual UINT16 GetRPNRegister(UINT16 reg);
	void NoteOn(UINT8 note, UINT8 vel, DRUMMAP* dm);
	void NoteOff(CSoundDevice* device, UINT8 ch);
};

class CMidiIn;

class CMidiInst {
protected:
	CMidiCh* ch[16];
	CMidiIn* Port;
	CFITOM* Parent;
	UINT16 currentstatus;
	UINT8 SysExBuf[MAX_SYSEX];
	UINT16 SysExPt;
	UINT8 MsgBuf[4];
	UINT16 MsgPt;
	enum CONDITION { COND_READY=0, COND_WAIT1=1, COND_WAIT2=2, COND_SYSEX=15, } cond;
	UINT16 ProcMsg(UINT8 msg);
	UINT16 SysEx();
	UINT16 Status3();
	UINT16 Status2();
	UINT16 Control();
	UINT16 SysMsg();
	UINT16 ChannelMap();
	BOOL clockenable;
public:
	CMidiInst(CMidiIn* port, CFITOM* parent, BOOL clkena);
	~CMidiInst();
	CMidiCh* GetMidiCh(UINT8 nch);
	void SetParent(CFITOM* parent);

	void AssignInst(UINT8 nch, CSoundDevice* dev, UINT8 pol);
	void AssignRhythm(UINT8 nch);
	void AssignThru(UINT8 nch, UINT8 och, UINT8 opt);
	void Release(UINT8 nch);
	int PollingCallBack();
	int InterruptCallBack(BYTE* buf, size_t length);
	void TimerCallBack(UINT32 tick);
	void MIDIClockCallBack(UINT32 tick);
	UINT16 SysExYamaha(BYTE* buf, size_t length);
	UINT16 SysExRoland(BYTE* buf, size_t length);
	UINT16 SysExUNRT(BYTE* buf, size_t length);
	UINT16 SysExURT(BYTE* buf, size_t length);
	const UINT16 GetCurrentStatus() { return currentstatus; };
};

class CThruCh : public CMidiCh {
protected:
	//Channel Status
	UINT8	LastNote;
	UINT8	ProgramNo;
	UINT16	PitchBend;
	//Control Change
	UINT8	Volume;
	UINT8	Panpot;
	UINT8	BankSelectMSB;
	UINT8	BankSelectLSB;
	UINT8	Expression;
	UINT8	PMDepth;
	UINT8	AMDepth;

	//RPN
	UINT8	BendRange;
	UINT16	Tuning;
	//NRPN
	UINT8	PMRate;
	UINT8	AMRate;

	//Internal
	UINT8	OutPort;
	UINT8	OutCh;
	UINT8	Poly;
	UINT8	Mono;
	UINT8	Port;
	UINT8	PortTime;
	UINT8	Legato;
	UINT8	Sustain;

public:
	CThruCh(UINT8 ch, UINT8 och, UINT8 opt, CFITOM* parent);
	~CThruCh() {};
	virtual void ProgChange(UINT8 prog) { ProgramNo = prog; };
	virtual void SetVolume(UINT8 vol) { Volume = vol; };
	virtual void SetExpress(UINT8 exp) { Expression = exp; };
	virtual void SetPitchBend(UINT16 pt) { PitchBend = pt; };
	virtual void SetPortamento(UINT8 on) { Port = on; };
	virtual void SetPortTime(UINT8 pt) { PortTime = pt; };
	virtual void SetFineTune(UINT16 tune) { Tuning = tune; };
	virtual void SetModulation(UINT8 dep) { PMDepth = dep; };
	virtual void SetFootCtrl(UINT8 dep) { AMDepth = dep; };
	virtual void SetBreathCtrl(UINT8 dep) {};
	virtual void SetPMRate(UINT8 rate) { PMRate = rate; };
	virtual void SetAMRate(UINT8 rate) { AMRate = rate; };
	virtual void SetPanpot(UINT8 pan) { Panpot = pan; };
	virtual void DataEntry(UINT8 data) {};
	virtual void SetPoly(UINT8 poly) { Poly = poly; Mono = 0; };
	virtual void SetMono(UINT8 mono) { Poly = 0; Mono = mono; };
	virtual void SetLegato(UINT8 leg) { Legato = leg; };
	virtual void SetSustain(UINT8 sus) { Sustain = sus; };
	virtual void BankSel(UINT16 bank) { BankSelectMSB = (bank >> 7) & 0x7f; BankSelectLSB = bank & 0x7f; };
	virtual void BankSelMSB(UINT8 bsm) { BankSelectMSB = bsm; };
	virtual void BankSelLSB(UINT8 bsl) { BankSelectLSB = bsl; };
	virtual void SetBendRange(UINT8 range) { BendRange = range; };
	virtual void NoteOn(UINT8 note, UINT8 vel) { LastNote = note; };
	virtual void NoteOff(UINT8 note) { LastNote = 0xff; };
	virtual void AllNoteOff() { LastNote = 0xff; };

	//Monitor
	virtual UINT32 GetDeviceID() const { return 0xff; };
	virtual UINT8 GetLastNote() const { return LastNote; };
	virtual UINT32 GetLastProg() const { return ProgramNo; };
	virtual UINT8 GetProgramNo() const { return ProgramNo; };
	virtual UINT16 GetBankNo() const { return BankSelectLSB; };
	virtual UINT8 GetTrackVolume() const { return Volume; };
	virtual UINT8 GetPoly() const { return Poly; };
	virtual UINT8 GetMono() const { return Mono; };
	virtual UINT8 GetPanpot() const { return Panpot; };
	virtual UINT8 GetExpress() const { return Expression; };
	virtual UINT8 GetModulation() const { return PMDepth; };
	virtual UINT8 GetFootCtrl() const { return AMDepth; };
	virtual UINT8 GetPMRate() const { return PMRate; };
	virtual UINT8 GetAMRate() const { return AMRate; };
	virtual UINT8 GetPortamento() const { return Port; };
	virtual UINT8 GetPortTime() const { return PortTime; };
	virtual UINT8 GetLegato() const { return Legato; };
	virtual UINT8 GetSustain() const { return Sustain; };
	virtual UINT16 GetPitchBend() const { return PitchBend; };
	virtual UINT8 GetBendRange() const { return BendRange; };

	virtual int IsThru() { return TRUE; }
	UINT8 GetOutPort() { return OutPort; };
	UINT8 GetOutCh() { return OutCh; };

	virtual void PollingCallBack() {};
	virtual void TimerCallBack(UINT32 tick) {};
};

#endif
