#include "stdafx.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "MIDIDEV.h"

static const FMVOICE NullVoice;

namespace ROM {
static int16_t portspeed[] = {
	6400,4799,3599,2699,2024,1518,1138,853,640,581,528,480,436,396,360,327,297,270,
	245,223,202,184,167,152,138,125,114,103,94,85,78,70,64,63,62,60,59,58,
	57,56,54,53,52,51,50,48,47,46,45,44,42,41,40,39,38,36,35,34,33,
	32,30,29,28,27,26,24,23,22,21,20,18,17,16,15,14,12,11,10,9,8,
	6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,
	4,4,4,4,3,3,3,3,3,3,2,2,2,1,1,1,1,-2,-2,-2,-3,-15
};

};

CInstCh::CPortaCtrl::CPortaCtrl()
{
	Init();
}

void CInstCh::CPortaCtrl::Init()
{
	start = end = current = 0xff;
	fine = 0;
	count = 0;
	speed = 0;
	enable = 0;
}

void CInstCh::CPortaCtrl::Start(uint8_t dst)
{
	if (current == 0xff) {
		current = (start != 0xff) ? start : dst;
		fine = 0;
	}
	end = dst;
	count = 0;
	status = RUNNING;
}

void CInstCh::CPortaCtrl::Stop()
{
	status = STOPPING;
}

void CInstCh::CPortaCtrl::Enable(int flag)
{
	Init();
	enable = uint8_t(flag);
}

void CInstCh::CPortaCtrl::SetSpeed(uint8_t sp)
{
	speed = sp;
}

void CInstCh::CPortaCtrl::SetSource(uint8_t src)
{
	Stop();
	current = 0xff;
	start = src;
	fine = 0;
}

void CInstCh::CPortaCtrl::Update()
{
	if (status == RUNNING) {
		count++;
		int16_t delta = ROM::portspeed[speed];
		int16_t absnote = (int16_t(current) << 6) | fine;
		int16_t target = (int16_t(end) << 6);
		if (delta < 0 && !(count % (-delta))) {
			delta = 1;
		}
		if (delta > 0) {
			int16_t remain = target - absnote;
			if (remain == 0) {
				status = STOPPING;
				count = 0;
			}
			else {
				if (remain < 0) {
					absnote = (delta < abs(remain)) ? (absnote - delta) : target;
				} else {
					absnote = (delta < abs(remain)) ? (absnote + delta) : target;
				}
				current = uint8_t(absnote >> 6);
				fine = uint8_t(absnote & 0x3f);
			}
		}
	}
}

//-----
CMidiInst::CMidiInst(CMidiIn* port, CFITOM* parent, bool clkena)
	: currentstatus(0), clockenable(clkena), bMidiProc(FALSE), bTimerProc(FALSE)
{
	Parent = parent;
	Port = port;
	for (int i=0; i<16; i++) {
		ch[i] = NULL;
	}
	cond = COND_READY;
	SysExPt = 0;
}

CMidiInst::~CMidiInst()
{
	for (int i=0; i<16; i++) {
		delete ch[i];
		ch[i] = 0;
	}
}

void CMidiInst::SetParent(CFITOM* parent)
{
	Parent = parent;
	for (int i = 0; i < 16; i++) {
		if (ch[i]) {
			ch[i]->SetParent(parent);
		}
	}
}

void CMidiInst::TimerCallBack(uint32_t tick)
{
	while (bMidiProc || bTimerProc);
	bTimerProc = TRUE;
	for (int i = 0; i < 16; i++) {
		if (ch[i]) {
			ch[i]->TimerCallBack(tick);
		}
	}
	bTimerProc = FALSE;
}

int CMidiInst::PollingCallBack()
{
	uint16_t ret = 0;
	bMidiProc = TRUE;
	while (Port->IsReceived()) {
		uint8_t msg = Port->Read();
		ret = ProcMsg(msg);
	}
	currentstatus = ret;
	bMidiProc = FALSE;
	return ret;
}

int CMidiInst::InterruptCallBack(BYTE* buf, size_t length)
{
	uint16_t ret = 0;
	while (bTimerProc || bMidiProc);
	bMidiProc = TRUE;
	while (length--) {
		ret = ProcMsg(*(buf++));
	}
	bMidiProc = FALSE;
	return ret;
}

void CMidiInst::MIDIClockCallBack(uint32_t tick)
{
	for (int i = 0; i<16; i++) {
		if (ch[i]) {
			ch[i]->MIDIClockCallBack(tick);
		}
	}
}

uint16_t CMidiInst::ProcMsg(uint8_t msg)
{
	uint16_t ret = 0;
	switch (cond) {
	case COND_SYSEX:
		if ((SysExPt + 1)<MAX_SYSEX) {
			SysExBuf[SysExPt++] = msg;
		}
		if (msg == 0xf7) { ret = SysEx(); }
		break;
	case COND_WAIT1:
		MsgBuf[1] = msg;
		ret = Status2();
		break;
	case COND_WAIT2:
		MsgBuf[2] = msg;
		ret = Status3();
		break;
	default://COND_READY
		switch (msg & 0xf0) {
			//case 0x00://Nothing
			//	break;
		case 0x80://Note on
		case 0x90://Note off
		case 0xa0://Note after touch
		case 0xb0://Control change
		case 0xc0://Program change
		case 0xd0://Channel after touch
		case 0xe0://Pitch bend
			cond = COND_WAIT1;
			ret = MsgBuf[0] = msg;
			//break;
			//Fall thru
		case 0xf0:
			MsgBuf[0] = msg;
			ret = SysMsg();
			break;
		default://Running status
			switch (MsgBuf[0] & 0xf0) {
			case 0xc0:
			case 0xd0:
			case 0xf0:
				//do nothing
				break;
			default:
				cond = COND_WAIT2;
				MsgBuf[1] = msg;
				ret = Status2();
			}
			break;
		}
		break;
	}
	return ret;
}

uint16_t CMidiInst::Status3()
{
	uint16_t ret = MsgBuf[0];
	uint16_t tmp;
	uint8_t midich = MsgBuf[0] & 0x0f;

	if (!ch[midich]) {
		AssignInst(midich, NULL, 0);
	}
	switch(MsgBuf[0] & 0xf0)
	{
	case 0x90:	// Note On
		if (MsgBuf[2]) {
			ch[midich]->NoteOn(MsgBuf[1], MsgBuf[2]);
			break;
		}//else thru fall
	case 0x80:	// Note Off
		if (!ch[midich]->IsRhythm()) {
			ch[midich]->NoteOff(MsgBuf[1]);
		}
		//ch[midich]->NoteOn(MsgBuf[1], 0);
		break;
	case 0xb0:	// Control Change / Mode Message
		ret = Control();
		break;
	case 0xe0:	// Pitch Bend
		tmp = MsgBuf[2] << 7 | MsgBuf[1];
		ch[midich]->SetPitchBend(tmp);
		break;
	case 0xf0:
	default:
		//currently ignore
		ret = 0;
		break;
	}
	if (ch[midich]->IsThru() && ret) {
		CThruCh* pch = (CThruCh*)ch[midich];
		CMidiOut* pmo = 0; // Parent->GetMidiOut(pch->GetOutPort());
		if (pmo) {
			uint8_t thrumsg[4];
			memcpy(thrumsg, MsgBuf, 3);
			thrumsg[0] = (MsgBuf[0] & 0xf0) | (pch->GetOutCh() & 0xf);
			pmo->Send(MsgBuf, 3);
		}
	}
	cond = COND_READY;
	return ret;
}

uint16_t CMidiInst::Status2()
{
	uint16_t ret = 0;
	uint8_t midich = MsgBuf[0] & 0x0f;

	if (!ch[midich]) {
		AssignInst(midich, NULL, 0);
	}
	switch(MsgBuf[0] & 0xf0)
	{
	case 0xc0:	//Program Change
		ch[midich]->ProgChange(MsgBuf[1]);
		cond = COND_READY;
		ret = MsgBuf[0];
		break;
	case 0xd0:	//Channel after touch
		cond = COND_READY;
		break;
	case 0xf0:
		switch (MsgBuf[0]) {
		case 0xf1:	//Time Code
		case 0xf3:	//Song Select
			cond = COND_READY;
			break;
		default:
			cond = COND_WAIT2;
			break;
		}
		break;
	default:
		cond = COND_WAIT2;
		break;
	}
	if (ch[midich]->IsThru() && ret) {
		CThruCh* pch = (CThruCh*)ch[midich];
		CMidiOut* pmo = 0; // Parent->GetMidiOut(pch->GetOutPort());
		if (pmo) {
			uint8_t thrumsg[4];
			memcpy(thrumsg, MsgBuf, 2);
			thrumsg[0] = (MsgBuf[0] & 0xf0) | (pch->GetOutCh() & 0xf);
			pmo->Send(thrumsg, 2);
		}
	}
	return ret;
}

uint16_t CMidiInst::Control()
{
	uint8_t midich = MsgBuf[0] & 0x0f;
	if (!ch[midich]) {
		AssignInst(midich, NULL, 0);
	}
	switch(MsgBuf[1])
	{
	case 0x00:	//Bank Select MSB
		if (MsgBuf[2] == DEVICE_RHYTHM && ch[midich]->IsInst()) {
			Release(midich);
			AssignRhythm(midich);
			break;
		}
		if (MsgBuf[2] != 0 && MsgBuf[2] != DEVICE_RHYTHM && ch[midich]->IsRhythm()) {
			Release(midich);
			AssignInst(midich, NULL, 0);
		}
		ch[midich]->BankSelMSB(MsgBuf[2]);
		break;
	case 0x01:	//Modulation Depth
		ch[midich]->SetModulation(MsgBuf[2]);
		break;
	case 0x04:	//Foot Controler Depth
		ch[midich]->SetFootCtrl(MsgBuf[2]);
		break;
	case 0x05:	//Portament Time
		ch[midich]->SetPortTime(MsgBuf[2]);
		break;
	case 0x06:	//Data Entry
		ch[midich]->DataEntry(MsgBuf[2]);
		break;
	case 0x07:	//Volume
		ch[midich]->SetVolume(MsgBuf[2]);
		break;
	case 0x0a:	//Panpot
		ch[midich]->SetPanpot(MsgBuf[2]);
		break;
	case 0x0b:	//Expression
		ch[midich]->SetExpress(MsgBuf[2]);
		break;
	case 0x20:	//Bank Select LSB
		ch[midich]->BankSelLSB(MsgBuf[2]);
		break;
	case 0x26:	//Data Entry LSB
		ch[midich]->DataEntryLSB(MsgBuf[2]);
		break;
	case 0x40:	//Suatain
		ch[midich]->SetSustain(MsgBuf[2]);
		break;
	case 0x41:	//Portament
		ch[midich]->SetPortamento(MsgBuf[2]);
		break;
	case 0x44:	//Legato
		ch[midich]->SetLegato(MsgBuf[2]);
		break;
	case 0x4f:	//Force release
		ch[midich]->SetForceDamp(MsgBuf[2]);
		break;
	case 0x54:
		ch[midich]->SetSourceNote(MsgBuf[2]);
	case 0x59:	//Voice Register Address
		ch[midich]->SetVoiceAddr(MsgBuf[2]);
		break;
	case 0x5a:	//Voice Register Data
		ch[midich]->SetVoiceData(MsgBuf[2]);
		break;
	case 0x60:	//Data Increment
		ch[midich]->DataIncrement(int16_t(uint16_t(MsgBuf[2])));
		break;
	case 0x61:	//Data Decrement
		ch[midich]->DataDecrement(int16_t(uint16_t(MsgBuf[2])));
		break;
	case 0x62:	//NRPN LSB
		ch[midich]->SetNRPNLSB(MsgBuf[2]);
		break;
	case 0x63:	//NRPN MSB
		ch[midich]->SetNRPNMSB(MsgBuf[2]);
		break;
	case 0x64:	//RPN LSB
		ch[midich]->SetRPNLSB(MsgBuf[2]);
		break;
	case 0x65:	//RPN MSB
		ch[midich]->SetRPNMSB(MsgBuf[2]);
		break;
	case 0x78:	//All Sound Off
	case 0x7b:	//All Note Off
	case 0x7c:	//Omni off
	case 0x7d:	//Omni on
		ch[midich]->AllNoteOff();
		break;
	case 0x79:	//Reset All Controller
		ch[midich]->ResetAllCtrl();
		break;
	case 0x7e:	//Mono on
		ch[midich]->SetMono(MsgBuf[2]);
		break;
	case 0x7f:	//Poly on
		ch[midich]->SetMono(0);
		break;
	}
	return MsgBuf[1];
}

uint16_t CMidiInst::SysEx()
{
	uint16_t ret = 0;
	int rdpt = 1;
	if (SysExBuf[0] == 0xf0 && SysExBuf[SysExPt - 1] == 0xf7) {
		DWORD mID = SysExBuf[rdpt++];
		if (mID == 0) {
			mID = mID | (SysExBuf[rdpt+1] << 8) | (SysExBuf[rdpt+2] << 16);
			rdpt += 2;
		}
		switch (mID) {
		case MID_ROLAND:
			ret = SysExRoland(&SysExBuf[rdpt], SysExPt - rdpt);
			break;
		case MID_YAMAHA:
			ret = SysExYamaha(&SysExBuf[rdpt], SysExPt - rdpt);
			break;
		case MID_UNRT:
			ret = SysExUNRT(&SysExBuf[rdpt], SysExPt - rdpt);
			break;
		case MID_URT:
			ret = SysExURT(&SysExBuf[rdpt], SysExPt - rdpt);
			break;
		}
	}
	cond = COND_READY;
	SysExPt = ret;
	return 0;
}

uint16_t CMidiInst::SysExRoland(BYTE* buf, size_t length)
{
	size_t rdpt = 0;
	BYTE devID = buf[rdpt++];	// device ID
	BYTE modID = buf[rdpt++];	// model ID
	BYTE comID = buf[rdpt++];	// command ID
	if (modID == 0x45 && comID == 0x12) {	// Roland LCD

	}
	return 0;
}

uint16_t CMidiInst::SysExYamaha(BYTE* buf, size_t length)
{
	return 0;
}

uint16_t CMidiInst::SysExURT(BYTE* buf, size_t length)
{
	size_t rdpt = 0;
	BYTE devID = buf[rdpt++];	// device ID
	BYTE sub1 = buf[rdpt++];
	BYTE sub2 = buf[rdpt++];
	if (sub1 == 0x04 && length > 4) {
		switch (sub2) {
		case	0x01:	// Master Volume
			Parent->SetMasterVolume(buf[rdpt+1]);
			break;
		}
	}
	return 0;
}

uint16_t CMidiInst::SysExUNRT(BYTE* buf, size_t length)
{
	return 0;
}

uint16_t CMidiInst::ChannelMap()
{
	return 0;
}

uint16_t CMidiInst::SysMsg()
{
	uint16_t ret = 0;
	switch (MsgBuf[0]) {
	case 0xf0://Exclusive message
		cond = COND_SYSEX;
		SysExBuf[SysExPt++] = MsgBuf[0];
		break;
	case 0xf1:	//MIDI Time code
	case 0xf2:	//Song Position
	case 0xf3:	//Song Select
	default:
		cond = COND_WAIT1;
		break;
	case 0xf7://End of SysEx
		if ((SysExPt+1)<MAX_SYSEX) {
			SysExBuf[SysExPt++] = MsgBuf[0];
		}
		ret = SysEx();
		break;
	case 0xf8://MIDI clock
		if (clockenable) {
			Parent->MIDIClockCallBack();
		}
		break;
	case 0xf4://undefined
	case 0xf5://undefined
	case 0xf6://Tune Request
	case 0xf9://undefined
	case 0xfa://SEQ start
	case 0xfb://SEQ continue
	case 0xfc://SEQ stop
	case 0xfd://undefined
	case 0xfe://Active sensing
	case 0xff://Reset
		cond = COND_READY;//currently ignore
		break;
	}
	return ret;
}

void CMidiInst::AssignInst(uint8_t nch, CSoundDevice* dev, uint8_t pol)
{
	if (nch < 16) {
		ch[nch] = new CInstCh(nch, Parent);
		ch[nch]->Assign(dev, pol);
	}
}

void CMidiInst::AssignRhythm(uint8_t nch)
{
	if (nch < 16) {
		ch[nch] = new CRhythmCh(nch, Parent);
	}
}

void CMidiInst::AssignThru(uint8_t nch, uint8_t och, uint8_t opt)
{
	if (nch < 16) {
		//ch[nch] = new CThruCh(nch, och, opt, Parent);
	}
}

CMidiCh* CMidiInst::GetMidiCh(uint8_t nch)
{
	CMidiCh* ret = NULL;
	if (nch < 16) {
		ret = ch[nch];
	}
	return ret;
}

void CMidiInst::Release(uint8_t nch)
{
	if (ch[nch]) {
		if (ch[nch]->IsInst()) {
			ch[nch]->AllNoteOff();
		}
		delete ch[nch];
		ch[nch] = NULL;
	}
}

/*------------------------------------------------------*/

CMidiCh::CMidiCh(uint8_t ch, CFITOM* parent) : Parent(parent)
{
	RPNReg = RPN_NULL;
	NRPNReg = RPN_NULL;
	EntryMode = 0;
}

void CMidiCh::SetNRPNLSB(uint8_t nrpn)
{
	NRPNReg = (NRPNReg & 0x3f80) | nrpn;
	DataLSB = 0;
	EntryMode = NRPN_ENTRY;
}

void CMidiCh::SetNRPNMSB(uint8_t nrpn)
{
	NRPNReg = (NRPNReg & 0x7f) | (nrpn << 7);
	DataLSB = 0;
	EntryMode = NRPN_ENTRY;
}

void CMidiCh::SetRPNLSB(uint8_t rpn)
{
	RPNReg = (RPNReg & 0x3f80) | rpn;
	DataLSB = 0;
	EntryMode = RPN_ENTRY;
}

void CMidiCh::SetRPNMSB(uint8_t rpn)
{
	RPNReg = (RPNReg & 0x7f) | (rpn << 7);
	DataLSB = 0;
	EntryMode = RPN_ENTRY;
}

void CMidiCh::DataEntry(uint8_t data)
{
	switch (EntryMode) {
	case NRPN_ENTRY:
		SetNRPNRegister(NRPNReg, (data << 7) | DataLSB);
		break;
	case RPN_ENTRY:
		SetRPNRegister(RPNReg, (data << 7) | DataLSB);
		break;
	}
}

void CMidiCh::DataIncrement(int16_t data)
{
	switch (EntryMode) {
	case NRPN_ENTRY:
		SetNRPNRegister(NRPNReg, min(GetNRPNRegister(NRPNReg) + 1, 127));
		break;
	case RPN_ENTRY:
		SetRPNRegister(RPNReg, min(GetRPNRegister(RPNReg) + 1, 127));
		break;
	}
}

void CMidiCh::DataDecrement(int16_t data)
{
	switch (EntryMode) {
	case NRPN_ENTRY:
		SetNRPNRegister(NRPNReg, max((int)GetNRPNRegister(NRPNReg) - 1, 0));
		break;
	case RPN_ENTRY:
		SetRPNRegister(RPNReg, max((int)GetRPNRegister(RPNReg) - 1, 0));
		break;
	}
}

void CMidiCh::AllNoteOff()
{
}

void CMidiCh::ResetAllCtrl()
{
	EntryMode = 0;
	RPNReg = RPN_NULL;
	NRPNReg = RPN_NULL;

	SetMono(0);
	SetLegato(0);
	SetPortamento(0);
	SetSustain(0);
	SetVolume(100);
	SetExpress(127);
	SetBendRange(2);
	SetPitchBend(8192);
	SetFineTune(8192);
	SetPanpot(64);
	BankSel(0);
	SetModulation(0);
	SetFootCtrl(0);
	SetBreathCtrl(0);
	SetPMRate(64);
	SetAMRate(64);
	ProgChange(0);
}

/*-----*/

CInstCh::CInstCh(uint8_t ch, CFITOM* parent) : CMidiCh(ch, parent)
{
	Device = NULL;
	voice = NullVoice;
	timbres = 0;

	ResetAllCtrl();
	Poly = 0;
	Mono = 0;
	Portamento.Init();
	PhyCh = 127;

	for (int i=0; i<16; i++)
	{
		Note[i].ch = 0xff;
		Note[i].fine = 0;
		Note[i].note = 0xff;
		Note[i].dev = 0;
	}
}

void CInstCh::ResetAllCtrl()
{
	CMidiCh::ResetAllCtrl();
	seqparam.mode = 0;
	seqparam.steps = 0;
	seqparam.tick = 0;
	seqparam.gate = 0;
	for (int i = 0; i < 16; i++) {
		seqparam.note[i] = 0;
	}
}


void CInstCh::Assign(CSoundDevice* dev, uint8_t pol)
{
	if (dev)
	{
		if (pol > dev->GetAvailable() || pol == 0) {
			pol = dev->GetAvailable();
		}
		Device = dev;
		Poly = pol;
		Mono = (Poly <= 1) ? 1 : 0;
		//BankSel(BankSelect);
		ProgChange(ProgramNo);
		PhyCh = 127;
	}
}

void CInstCh::BankSelLSB(uint8_t bsl)
{
	BankSelL = bsl;
	if (BankSelM == ADPCM_BANK) { //ADPCM
		BankSelMSB(ADPCM_BANK);
	}
}

void CInstCh::BankSelMSB(uint8_t bsm)
{
	if (bsm) {
		BankSelM = bsm;
		CSoundDevice* dev = (bsm == ADPCM_BANK) ? (CSoundDevice*)Parent->GetPCMDeviceFromIndex(BankSelL) : Parent->GetLogicalDeviceFromID(bsm);
		if (dev) {
			//AllNoteOff();
			Assign(dev, dev->GetAvailable());
			//ProgChange(ProgramNo);	// Reload
		}
	}
}

void CInstCh::BankSel(uint16_t bank)
{
	BankSelM = bank >> 8;
	BankSelL = bank & 0xff;
}

void CInstCh::ProgChange(uint8_t prog)
{
	ProgramNo = prog;
	if (Device)
	{
		Parent->GetVoice(&voice, Device->GetDevice(), BankSelL, prog);
		for (int i=0; i<timbres; i++) {
			if (Note[i].dev == Device) {
				Device->SetVoice(Note[i].ch, &voice);
			}
		}
	}
}

void CInstCh::NoteOn(uint8_t note, uint8_t vel)
{
	uint8_t ch = 0xff;
	if (Device)
	{
		uint8_t noteon = 1;
		uint16_t id = 0;

		if (PhyCh != 127 && PhyCh < Device->GetChs()) {
			ch = Device->Assign(PhyCh, this, &voice);
		}
		else {
			if (Mono && timbres) {
				ch = Note[0].ch;
				if (Note[0].note != uint8_t(-1) && !Legato) {
					Note[0].dev->NoteOff(ch);
				}
				if (Legato) {
					noteon = 0;
				}
			}
			else {
				ch = Device->AllocCh(this, &voice);
			}
		}
		if (ch != 0xff) {
			if (PMDepth > 0) {
				Device->SetDevAMDepth(ch, 0);
				Device->SetDevPMDepth(ch, PMDepth);
				Device->SetDevPMRate(ch, PMRate);
			}
			else if (AMDepth > 0) {
				Device->SetDevPMDepth(ch, 0);
				Device->SetDevAMDepth(ch, AMDepth);
				Device->SetDevAMRate(ch, AMRate);
			}
			else {
				Device->SetDevAMDepth(ch, 0);
				Device->SetDevPMDepth(ch, 0);
				Device->SetDevPMRate(ch, 0);
				Device->SetDevAMRate(ch, 0);
			}
			Device->SetVolume(ch, Volume, 0);
			Device->SetExpress(ch, Expression, 0);
			Device->SetSustain(ch, Sustain, 0);
			Device->SetPanpot(ch, Panpot);

			Enter(ch, note, id);
			if (Portamento.IsEnable()) {
				Portamento.Start(note);
			}
			else {
				int fine = int16_t(BendRange) * (int16_t(PitchBend >> 7) - 64) + (int16_t(Tuning >> 7) - 64);
				Device->SetNoteFine(ch, note, fine, 1);
			}

			if (noteon) {
				Device->NoteOn(ch, vel);
			}
			else {
				Device->SetVelocity(ch, vel);
			}
		}
	}
}

void CInstCh::NoteOff(uint8_t note)
{
	if (Device && !(Mono && Legato)) {
		for (int i=0; i<timbres; i++) {
			if ((note == 255 || Note[i].note == note) && Note[i].dev) {
				Note[i].dev->NoteOff(Note[i].ch);
				Note[i].dev->ReleaseCh(Note[i].ch);
				Leave(i);
				break;
			}
		}
	}
}

void CInstCh::AllNoteOff()
{
	if (Device && timbres) {
		for (int i=timbres-1; i>=0; i--) {
			Device->NoteOff(Note[i].ch);
			Device->ReleaseCh(Note[i].ch);
		}
		Portamento.Stop();
	}
	timbres = 0;
}

void CInstCh::SetModulation(uint8_t dep)
{
	PMDepth = dep;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetDevPMDepth(Note[i].ch, dep);
		}
	}
}

void CInstCh::SetFootCtrl(uint8_t dep)
{
	AMDepth = dep;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetDevAMDepth(Note[i].ch, dep);
		}
	}
}

void CInstCh::SetBreathCtrl(uint8_t dep)
{
}

void CInstCh::SetExpress(uint8_t exp)
{
	Expression = exp;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetExpress(Note[i].ch, exp);
		}
	}
}

void CInstCh::SetVolume(uint8_t vol)
{
	Volume = vol;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetVolume(Note[i].ch, vol);
		}
	}
}

void CInstCh::SetPanpot(uint8_t pan)
{
	Panpot = pan;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetPanpot(Note[i].ch, pan);
		}
	}
}

void CInstCh::UpdateFineTune()
{
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Note[i].fine = int16_t(BendRange) * (int16_t(PitchBend>>7)-64) + (int16_t(Tuning>>7)-64);
			if (Portamento.IsEnable()) {
				Device->SetNoteFine(Note[i].ch, Portamento.GetCurrentNote(), Portamento.GetCurrentFine()+Note[i].fine, 1);
			} else {
				Device->SetNoteFine(Note[i].ch, Note[i].note, Note[i].fine, 1);
			}
		}
	}
}

void CInstCh::SetPitchBend(uint16_t pt)
{
	PitchBend = pt;
	UpdateFineTune();
}

void CInstCh::SetFineTune(uint16_t tune)
{
	Tuning = tune;
	UpdateFineTune();
}

void CInstCh::SetBendRange(uint8_t range)
{
	BendRange = range;
	UpdateFineTune();
}

void CInstCh::SetPMRate(uint8_t rate)
{
	PMRate = rate;
	if (Device) {
		Device->SetDevAMRate(0, rate);
	}
}

void CInstCh::SetAMRate(uint8_t rate)
{
	AMRate = rate;
	if (Device) {
		Device->SetDevPMRate(0, rate);
	}
}

void CInstCh::SetNRPNRegister(uint16_t reg, uint16_t data)
{
	if (!Device) { return; }
	switch(NRPNReg) {
	case NRPN_PMRATE:
		SetPMRate(data >> 7);
		break;
	case NRPN_AMRATE:
		SetAMRate(data >> 7);
		break;
	case NRPN_PHYCH:
		SetPhyCh(data >> 7);
		break;
	case NRPN_DIRADR:
		SetRegAddr(data >> 7);
		break;
	case NRPN_DIRDAT:
		SetRegData(data >> 7);
		break;
	default:
		break;
	}
}

void CInstCh::SetRPNRegister(uint16_t reg, uint16_t data)
{
	switch (RPNReg)
	{
	case RPN_PITCHBEND:
		SetBendRange(data >> 7);
		break;
	case RPN_FINETUNE:
		SetFineTune(data);
		break;
	}
}

uint16_t CInstCh::GetNRPNRegister(uint16_t reg)
{
	uint16_t ret = 0;
	switch (NRPNReg) {
	case NRPN_PMRATE:
		ret = GetPMRate() << 7;
		break;
	case NRPN_AMRATE:
		ret = GetAMRate() << 7;
		break;
	case NRPN_PHYCH:
		ret = GetPhyCh() << 7;
		break;
	case NRPN_DIRADR:
		ret = GetRegAddr() << 7;
		break;
	case NRPN_DIRDAT:
		ret = GetRegData() << 7;
		break;
	}
	return ret;
}

uint16_t CInstCh::GetRPNRegister(uint16_t reg)
{
	uint16_t ret = 0;
	switch (RPNReg)
	{
	case RPN_PITCHBEND:
		ret = GetBendRange() << 7;
		break;
	case RPN_FINETUNE:
		ret = GetFineTune();
		break;
	}
	return ret;
}

void CInstCh::SetRegAddr(uint8_t addr)
{
	DirAddr = (uint16_t(addr) << 7) | (DataLSB & 0x7f);
}

void CInstCh::SetRegData(uint8_t data)
{
	DirData = (uint16_t(data) << 7) | (DataLSB & 0x7f);
	if (Device) {
		Device->SetReg(DirAddr, uint8_t(DirData));
	}
}

uint8_t CInstCh::GetRegData() const
{
	if (Device) {
		return Device->GetReg(DirAddr, 0);
	}
	return 0;
}

void CInstCh::Enter(uint8_t ch, uint8_t note, uint16_t count)
{
	if (Mono) { timbres = 0; }
	if (timbres < 15) {
		Note[timbres].ch = ch;
		Note[timbres].note = note;
		Note[timbres].fine = int16_t(BendRange) * (int16_t(PitchBend>>7)-64) + (int16_t(Tuning>>7)-64);
		Note[timbres].dev = Device;
		timbres++;
	}
}

void CInstCh::Leave(uint8_t idx)
{
	if (timbres) {
		for (int i=idx; i<(timbres-1); i++) {
			Note[i] = Note[i+1];
		}
		timbres--;
		Note[timbres].ch = (uint8_t)-1;
		Note[timbres].note = (uint8_t)-1;
		Note[timbres].fine = 0;
		Note[timbres].dev = 0;
	}
}

uint32_t CInstCh::GetDeviceID() const
{
	uint32_t ret = 0;
	if (Device) {
		if (Parent->isPcmDevice(Device)) {
			ret = Parent->GetDeviceUniqID((CAdPcmBase*)Device);
		}
		else {
			ret = Parent->GetDeviceUniqID(Device);
		}
	}
	return ret;	
}

uint8_t CInstCh::GetLastNote() const
{
	uint8_t ret = 0xff;
	if (timbres) {
		ret = Note[timbres-1].note;
	}
	return ret;
}

uint32_t CInstCh::GetLastProg() const
{
	return voice.ID;
}

void CInstCh::SetPoly(uint8_t poly)
{
	if (poly > 1) {
		if (Device) {
			AllNoteOff();
			Poly = min(poly, Device->GetAvailable());
			Mono = 0;
			SetPortamento(0);
			SetLegato(0);
		}
	} else {
		if (Device) {
			AllNoteOff();
		}
		Poly = poly;
		Mono = 1;
	}
	PhyCh = 127;
}

void CInstCh::SetPhyCh(uint8_t ch)
{
	if (Device) {
		PhyCh = 127;
		//AllNoteOff();
		if (ch != 127 && ch < Device->GetChs()) {
			PhyCh = ch;
			Poly = 1;
			Mono = 1;
		}
	}
}

void CInstCh::SetMono(uint8_t mono)
{
	if (!mono) { mono = 255; }
	SetPoly(mono);
}

void CInstCh::SetLegato(uint8_t leg)
{
	Legato = Mono ? ((leg < 64) ? 0 : 1) : 0;
}

void CInstCh::SetSustain(uint8_t sus)
{
	Sustain = (sus < 64) ? 0 : 1;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetSustain(Note[i].ch, Sustain);
		}
	}
}

void CInstCh::SetForceDamp(uint8_t dmp)
{
	ForceDamp = (dmp < 64) ? 0 : 1;
}

void CInstCh::SetSourceNote(uint8_t note)
{
	Portamento.SetSource(note);
}

void CInstCh::SetPortamento(uint8_t on)
{
	if (on > 63) {
		if (Mono) {
			Portamento.Enable(1);
		}
	} else {
		if (Portamento.IsEnable()) {
			Portamento.Stop();
		}
		Portamento.Enable(0);
	}
}

void CInstCh::SetPortTime(uint8_t pt)
{
	Portamento.SetSpeed(pt);
}

void CInstCh::SetVoiceData(uint8_t data)
{
	if (Device)
	{
		((uint8_t*)&voice)[VoiceAddr&0x7f] = data;
		Device->ResetVoice(this, &voice, -1);
		/*
		for (int i=0; i<timbres; i++) {
			Device->SetVoice(Note[i].ch, &voice, -1);
		}
		*/
	}
}

void CInstCh::SetVoiceData(FMVOICE* data)
{
	if (Device && data)
	{
		voice = *data;
		Device->ResetVoice(this, &voice, -1);
		/*
		for (int i=0; i<timbres; i++) {
			Device->SetVoice(Note[i].ch, &voice, -1);
		}
		*/
	}
}

void CInstCh::TimerCallBack(uint32_t tick)
{
	if (Portamento.IsEnable()) {
		Portamento.Update();
		UpdateFineTune();
	}
}

void CInstCh::MIDIClockCallBack(uint32_t tick)
{

}

//--------------------
CRhythmCh::CRhythmCh(uint8_t ch, CFITOM* parent) : CMidiCh(ch, parent)
{
	LastNote = 0xff;
	Volume = 100;
	ProgramNo = 0;
	memset(Note, 0, sizeof(Note));
}

void CRhythmCh::ResetAllCtrl()
{
	CMidiCh::ResetAllCtrl();
	InitInstant();
}

void CRhythmCh::InitInstant()
{
	for (int i = 0; i < 128; i++) {
		instant[i].pan = 0;
		instant[i].pitch = 0;
		instant[i].vel = 0;
	}
}

void CRhythmCh::ProgChange(uint8_t prog)
{
	ProgramNo = prog & 0x7f;
	InitInstant();
}

void CRhythmCh::SetVolume(uint8_t vol)
{
	Volume = vol & 0x7f;
}

void CRhythmCh::NoteOn(uint8_t note, uint8_t vel)
{
	DRUMMAP dm;
	int res = Parent->GetDrum(&dm, 0, uint8_t(ProgramNo), note);
	if (!res && dm.device) {
		NoteOn(note, vel, &dm);
	}
}

void CRhythmCh::NoteOn(uint8_t note, uint8_t vel, DRUMMAP* dm)
{
	FMVOICE dv;
	if (note < 128 && dm && dm->device) {
		LastNote = note;
		if (Note[note].device) {
			NoteOff(note);
		}
		//Inst Rhythm
		Parent->GetVoice(&dv, dm->devID, dm->bank, dm->prog);
		uint8_t ch = dm->device->AllocCh(this, &dv);
		if (ch != 0xff) {
			dm->device->SetExpress(ch, 127);
			dm->device->SetVolume(ch, Volume, 0);
			dm->device->SetPanpot(ch, dm->pan + 64);
			dm->device->SetNoteFine(ch, dm->num, (int16_t)dm->fnum);
			dm->device->NoteOn(ch, vel);
			Note[note].device = dm->device;
			Note[note].ch = ch;
			Note[note].note = dm->num;
			Note[note].count = dm->gate;
		}
	}
}

void CRhythmCh::NoteOff(uint8_t note)
{
	LastNote = 0xff;
	if (note < 128 && Note[note].device) {
		Note[note].device->NoteOff(Note[note].ch);
		Note[note].device->ReleaseCh(Note[note].ch);
		Note[note].device = 0;
		Note[note].ch = 0;
		Note[note].note = 0;
		Note[note].count = 0;
	}
}

void CRhythmCh::NoteOff(CSoundDevice* device, uint8_t ch)
{
	for (int i=0; i<128; i++) {
		if (Note[i].device == device && Note[i].ch == ch) {
			NoteOff(i);
			break;
		}
	}
}

void CRhythmCh::AllNoteOff()
{
	for (int i=0; i<128; i++) {
		if (Note[i].device) {
			NoteOff(i);
		}
	}
}

void CRhythmCh::SetNRPNRegister(uint16_t reg, uint16_t data)
{
	switch (NRPNReg & 0x7f00) {
	case NRPN_DRUM_PITCH:
		break;
	case NRPN_DRUM_LEVEL:
		break;
	case NRPN_DRUM_PAN:
		break;
	}
}

void CRhythmCh::SetRPNRegister(uint16_t reg, uint16_t data)
{
}

uint16_t CRhythmCh::GetNRPNRegister(uint16_t reg)
{
	uint16_t ret = 0;
	switch (NRPNReg & 0x7f00) {
	case NRPN_DRUM_PITCH:
		break;
	case NRPN_DRUM_LEVEL:
		break;
	case NRPN_DRUM_PAN:
		break;
	}
	return ret;
}

uint16_t CRhythmCh::GetRPNRegister(uint16_t reg)
{
	uint16_t ret = 0;
	return ret;
}

void CRhythmCh::TimerCallBack(uint32_t tick)
{
	for (int i=0; i<128; i++) {
		if (Note[i].device) {
			if (Note[i].count) {
				if (--Note[i].count == 0) {
					NoteOff(i);
				}
			}
			else {
			}
		}
	}
}

/*----------*/

CThruCh::CThruCh(uint8_t ch, uint8_t och, uint8_t opt, CFITOM* parent) : CMidiCh(ch, parent), OutPort(opt), OutCh(och)
{
	ResetAllCtrl();
	Poly = 0;
	Mono = 0;
	Port = 0;
	PortTime = 0;
}
