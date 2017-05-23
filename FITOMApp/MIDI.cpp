#include "stdafx.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "MIDIDEV.h"

static const FMVOICE NullVoice;

namespace ROM {
static SINT16 portspeed[] = {
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

void CInstCh::CPortaCtrl::Start(UINT8 dst)
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
	enable = UINT8(flag);
}

void CInstCh::CPortaCtrl::SetSpeed(UINT8 sp)
{
	speed = sp;
}

void CInstCh::CPortaCtrl::SetSource(UINT8 src)
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
		SINT16 delta = ROM::portspeed[speed];
		SINT16 absnote = (SINT16(current) << 6) | fine;
		SINT16 target = (SINT16(end) << 6);
		if (delta < 0 && !(count % (-delta))) {
			delta = 1;
		}
		if (delta > 0) {
			SINT16 remain = target - absnote;
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
				current = UINT8(absnote >> 6);
				fine = UINT8(absnote & 0x3f);
			}
		}
	}
}

//-----
CMidiInst::CMidiInst(CMidiIn* port, CFITOM* parent, BOOL clkena)
	: currentstatus(0), clockenable(clkena)
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

void CMidiInst::TimerCallBack(UINT32 tick)
{
	for (int i=0; i<16; i++) {
		if (ch[i]) {
			ch[i]->TimerCallBack(tick);
		}
	}
}

int CMidiInst::PollingCallBack()
{
	UINT16 ret = 0;
	while (Port->IsReceived()) {
		UINT8 msg = Port->Read();
		ret = ProcMsg(msg);
	}
	currentstatus = ret;
	return ret;
}

int CMidiInst::InterruptCallBack(BYTE* buf, size_t length)
{
	UINT16 ret = 0;
	while (length--) {
		ret = ProcMsg(*(buf++));
	}
	return ret;
}

void CMidiInst::MIDIClockCallBack(UINT32 tick)
{
	for (int i = 0; i<16; i++) {
		if (ch[i]) {
			ch[i]->MIDIClockCallBack(tick);
		}
	}
}

UINT16 CMidiInst::ProcMsg(UINT8 msg)
{
	UINT16 ret = 0;
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

UINT16 CMidiInst::Status3()
{
	UINT16 ret = MsgBuf[0];
	UINT16 tmp;
	UINT8 midich = MsgBuf[0] & 0x0f;

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
			UINT8 thrumsg[4];
			memcpy(thrumsg, MsgBuf, 3);
			thrumsg[0] = (MsgBuf[0] & 0xf0) | (pch->GetOutCh() & 0xf);
			pmo->Send(MsgBuf, 3);
		}
	}
	cond = COND_READY;
	return ret;
}

UINT16 CMidiInst::Status2()
{
	UINT16 ret = 0;
	UINT8 midich = MsgBuf[0] & 0x0f;

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
			UINT8 thrumsg[4];
			memcpy(thrumsg, MsgBuf, 2);
			thrumsg[0] = (MsgBuf[0] & 0xf0) | (pch->GetOutCh() & 0xf);
			pmo->Send(thrumsg, 2);
		}
	}
	return ret;
}

UINT16 CMidiInst::Control()
{
	UINT8 midich = MsgBuf[0] & 0x0f;
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
		ch[midich]->DataIncrement(SINT16(UINT16(MsgBuf[2])));
		break;
	case 0x61:	//Data Decrement
		ch[midich]->DataDecrement(SINT16(UINT16(MsgBuf[2])));
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

UINT16 CMidiInst::SysEx()
{
	UINT16 ret = 0;
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

UINT16 CMidiInst::SysExRoland(BYTE* buf, size_t length)
{
	size_t rdpt = 0;
	BYTE devID = buf[rdpt++];	// device ID
	BYTE modID = buf[rdpt++];	// model ID
	BYTE comID = buf[rdpt++];	// command ID
	if (modID == 0x45 && comID == 0x12) {	// Roland LCD

	}
	return 0;
}

UINT16 CMidiInst::SysExYamaha(BYTE* buf, size_t length)
{
	return 0;
}

UINT16 CMidiInst::SysExURT(BYTE* buf, size_t length)
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

UINT16 CMidiInst::SysExUNRT(BYTE* buf, size_t length)
{
	return 0;
}

UINT16 CMidiInst::ChannelMap()
{
	return 0;
}

UINT16 CMidiInst::SysMsg()
{
	UINT16 ret = 0;
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

void CMidiInst::AssignInst(UINT8 nch, CSoundDevice* dev, UINT8 pol)
{
	if (nch < 16) {
		ch[nch] = new CInstCh(nch, Parent);
		ch[nch]->Assign(dev, pol);
	}
}

void CMidiInst::AssignRhythm(UINT8 nch)
{
	if (nch < 16) {
		ch[nch] = new CRhythmCh(nch, Parent);
	}
}

void CMidiInst::AssignThru(UINT8 nch, UINT8 och, UINT8 opt)
{
	if (nch < 16) {
		//ch[nch] = new CThruCh(nch, och, opt, Parent);
	}
}

CMidiCh* CMidiInst::GetMidiCh(UINT8 nch)
{
	CMidiCh* ret = NULL;
	if (nch < 16) {
		ret = ch[nch];
	}
	return ret;
}

void CMidiInst::Release(UINT8 nch)
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

CMidiCh::CMidiCh(UINT8 ch, CFITOM* parent) : Parent(parent)
{
	RPNReg = RPN_NULL;
	NRPNReg = RPN_NULL;
	EntryMode = 0;
}

void CMidiCh::SetNRPNLSB(UINT8 nrpn)
{
	NRPNReg = (NRPNReg & 0x3f80) | nrpn;
	DataLSB = 0;
	EntryMode = NRPN_ENTRY;
}

void CMidiCh::SetNRPNMSB(UINT8 nrpn)
{
	NRPNReg = (NRPNReg & 0x7f) | (nrpn << 7);
	DataLSB = 0;
	EntryMode = NRPN_ENTRY;
}

void CMidiCh::SetRPNLSB(UINT8 rpn)
{
	RPNReg = (RPNReg & 0x3f80) | rpn;
	DataLSB = 0;
	EntryMode = RPN_ENTRY;
}

void CMidiCh::SetRPNMSB(UINT8 rpn)
{
	RPNReg = (RPNReg & 0x7f) | (rpn << 7);
	DataLSB = 0;
	EntryMode = RPN_ENTRY;
}

void CMidiCh::DataEntry(UINT8 data)
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

void CMidiCh::DataIncrement(SINT16 data)
{
	switch (EntryMode) {
	case NRPN_ENTRY:
		SetNRPNRegister(NRPNReg, max(GetNRPNRegister(NRPNReg) + data, 127));
		break;
	case RPN_ENTRY:
		SetRPNRegister(RPNReg, max(GetRPNRegister(RPNReg) + data, 127));
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

CInstCh::CInstCh(UINT8 ch, CFITOM* parent) : CMidiCh(ch, parent)
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


void CInstCh::Assign(CSoundDevice* dev, UINT8 pol)
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

void CInstCh::BankSelLSB(UINT8 bsl)
{
	BankSelL = bsl;
	if (BankSelM == ADPCM_BANK) { //ADPCM
		BankSelMSB(ADPCM_BANK);
	}
}

void CInstCh::BankSelMSB(UINT8 bsm)
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

void CInstCh::BankSel(UINT16 bank)
{
	BankSelM = bank >> 8;
	BankSelL = bank & 0xff;
}

void CInstCh::ProgChange(UINT8 prog)
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

void CInstCh::NoteOn(UINT8 note, UINT8 vel)
{
	UINT8 ch = 0xff;
	if (Device)
	{
		UINT8 noteon = 1;
		UINT16 id = 0;

		if (PhyCh != 127 && PhyCh < Device->GetChs()) {
			ch = Device->Assign(PhyCh, this, &voice);
		}
		else {
			if (Mono && timbres) {
				ch = Note[0].ch;
				if (Note[0].note != UINT8(-1) && !Legato) {
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
			Device->SetDevAMDepth(ch, AMDepth);
			Device->SetDevPMDepth(ch, PMDepth);
			Device->SetDevAMRate(ch, AMRate);
			Device->SetDevPMRate(ch, PMRate);
			Device->SetVolume(ch, Volume, 0);
			Device->SetExpress(ch, Expression, 0);
			Device->SetSustain(ch, Sustain, 0);
			Device->SetPanpot(ch, Panpot);

			Enter(ch, note, id);
			UpdateFineTune();

			if (noteon) {
				Device->NoteOn(ch, vel);
			}
			else {
				Device->SetVelocity(ch, vel);
			}
		}
	}
}

void CInstCh::NoteOff(UINT8 note)
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

void CInstCh::SetModulation(UINT8 dep)
{
	PMDepth = dep;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetDevPMDepth(Note[i].ch, dep);
		}
	}
}

void CInstCh::SetFootCtrl(UINT8 dep)
{
	AMDepth = dep;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetDevAMDepth(Note[i].ch, dep);
		}
	}
}

void CInstCh::SetBreathCtrl(UINT8 dep)
{
}

void CInstCh::SetExpress(UINT8 exp)
{
	Expression = exp;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetExpress(Note[i].ch, exp);
		}
	}
}

void CInstCh::SetVolume(UINT8 vol)
{
	Volume = vol;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetVolume(Note[i].ch, vol);
		}
	}
}

void CInstCh::SetPanpot(UINT8 pan)
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
			Note[i].fine = SINT16(BendRange) * (SINT16(PitchBend>>7)-64) + (SINT16(Tuning>>7)-64);
			if (Portamento.IsEnable()) {
				Device->SetNoteFine(Note[i].ch, Portamento.GetCurrentNote(), Portamento.GetCurrentFine()+Note[i].fine, 1);
			} else {
				Device->SetNoteFine(Note[i].ch, Note[i].note, Note[i].fine, 1);
			}
		}
	}
}

void CInstCh::SetPitchBend(UINT16 pt)
{
	PitchBend = pt;
	UpdateFineTune();
}

void CInstCh::SetFineTune(UINT16 tune)
{
	Tuning = tune;
	UpdateFineTune();
}

void CInstCh::SetBendRange(UINT8 range)
{
	BendRange = range;
	UpdateFineTune();
}

void CInstCh::SetPMRate(UINT8 rate)
{
	PMRate = rate;
	if (Device) {
		Device->SetDevAMRate(0, rate);
	}
}

void CInstCh::SetAMRate(UINT8 rate)
{
	AMRate = rate;
	if (Device) {
		Device->SetDevPMRate(0, rate);
	}
}

void CInstCh::SetNRPNRegister(UINT16 reg, UINT16 data)
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
	}
}

void CInstCh::SetRPNRegister(UINT16 reg, UINT16 data)
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

UINT16 CInstCh::GetNRPNRegister(UINT16 reg)
{
	UINT16 ret = 0;
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

UINT16 CInstCh::GetRPNRegister(UINT16 reg)
{
	UINT16 ret = 0;
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

void CInstCh::SetRegAddr(UINT8 addr)
{
	DirAddr = (UINT16(addr) << 7) | (DataLSB & 0x7f);
}

void CInstCh::SetRegData(UINT8 data)
{
	DirData = (UINT16(data) << 7) | (DataLSB & 0x7f);
	if (Device) {
		Device->SetReg(DirAddr, UINT8(DirData));
	}
}

UINT8 CInstCh::GetRegData() const
{
	if (Device) {
		return Device->GetReg(DirAddr, 0);
	}
	return 0;
}

void CInstCh::Enter(UINT8 ch, UINT8 note, UINT16 count)
{
	if (Mono) { timbres = 0; }
	if (timbres < 15) {
		Note[timbres].ch = ch;
		Note[timbres].note = note;
		Note[timbres].fine = SINT16(BendRange) * (SINT16(PitchBend>>7)-64) + (SINT16(Tuning>>7)-64);
		Note[timbres].dev = Device;
		timbres++;
	}
}

void CInstCh::Leave(UINT8 idx)
{
	if (timbres) {
		for (int i=idx; i<(timbres-1); i++) {
			Note[i] = Note[i+1];
		}
		timbres--;
		Note[timbres].ch = (UINT8)-1;
		Note[timbres].note = (UINT8)-1;
		Note[timbres].fine = 0;
		Note[timbres].dev = 0;
	}
}

UINT32 CInstCh::GetDeviceID() const
{
	UINT32 ret = 0;
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

UINT8 CInstCh::GetLastNote() const
{
	UINT8 ret = 0xff;
	if (timbres) {
		ret = Note[timbres-1].note;
	}
	return ret;
}

UINT32 CInstCh::GetLastProg() const
{
	return voice.ID;
}

void CInstCh::SetPoly(UINT8 poly)
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

void CInstCh::SetPhyCh(UINT8 ch)
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

void CInstCh::SetMono(UINT8 mono)
{
	if (!mono) { mono = 255; }
	SetPoly(mono);
}

void CInstCh::SetLegato(UINT8 leg)
{
	Legato = Mono ? ((leg < 64) ? 0 : 1) : 0;
}

void CInstCh::SetSustain(UINT8 sus)
{
	Sustain = (sus < 64) ? 0 : 1;
	if (Device) {
		for (int i=0; i<timbres; i++) {
			Device->SetSustain(Note[i].ch, Sustain);
		}
	}
}

void CInstCh::SetForceDamp(UINT8 dmp)
{
	ForceDamp = (dmp < 64) ? 0 : 1;
}

void CInstCh::SetSourceNote(UINT8 note)
{
	Portamento.SetSource(note);
}

void CInstCh::SetPortamento(UINT8 on)
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

void CInstCh::SetPortTime(UINT8 pt)
{
	Portamento.SetSpeed(pt);
}

void CInstCh::SetVoiceData(UINT8 data)
{
	if (Device)
	{
		((UINT8*)&voice)[VoiceAddr&0x7f] = data;
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

void CInstCh::TimerCallBack(UINT32 tick)
{
	if (Portamento.IsEnable()) {
		Portamento.Update();
		UpdateFineTune();
	}
}

void CInstCh::MIDIClockCallBack(UINT32 tick)
{

}

//--------------------
CRhythmCh::CRhythmCh(UINT8 ch, CFITOM* parent) : CMidiCh(ch, parent)
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

void CRhythmCh::ProgChange(UINT8 prog)
{
	ProgramNo = prog & 0x7f;
	InitInstant();
}

void CRhythmCh::SetVolume(UINT8 vol)
{
	Volume = vol & 0x7f;
	for (int i=0; i<Parent->GetLogDevs(); i++) {
		CSoundDevice* dev = Parent->GetLogicalDeviceFromIndex(i);
		if (dev) {
			dev->SetRhythmVol(Volume);
		}
	}
}

void CRhythmCh::NoteOn(UINT8 note, UINT8 vel)
{
	DRUMMAP dm;
	int res = Parent->GetDrum(&dm, 0, UINT8(ProgramNo), note);
	if (!res && dm.device) {
		NoteOn(note, vel, &dm);
	}
}

void CRhythmCh::NoteOn(UINT8 note, UINT8 vel, DRUMMAP* dm)
{
	LastNote = note;
	FMVOICE dv;
	if (dm && dm->device) {
		if (Note[note].device) {
			NoteOff(note);
		}
		if (dm->num & 0x80) { //Internal Rhythm
			ISoundDevice::FNUM fnum(dm->fnum>>12, dm->fnum&0x7ff);
			dm->num &= 0x7f;
			if (dm->bank < MAX_BANK && dm->prog < 128) {//for OPL
				Parent->GetVoice(&dv, dm->devID, dm->bank, dm->prog);
			}
			dm->device->SetRhythmVol(Volume);
			dm->device->RhythmOn(dm->num, vel, dm->pan, &dv, &fnum);
			Note[note].device = dm->device;
			Note[note].ch = 255;
			Note[note].note = dm->num;
			Note[note].count = dm->gate;
		} else { //Inst Rhythm
			Parent->GetVoice(&dv, dm->devID, dm->bank, dm->prog);
			UINT8 ch = dm->device->AllocCh(this, &dv);
			if (ch != 0xff) {
				dm->device->SetExpress(ch, 127);
				dm->device->SetVolume(ch, Volume, 0);
				dm->device->SetPanpot(ch, dm->pan + 64);
				dm->device->SetNoteFine(ch, dm->num, (SINT16)dm->fnum);
				dm->device->NoteOn(ch, vel);
				Note[note].device = dm->device;
				Note[note].ch = ch;
				Note[note].note = dm->num;
				Note[note].count = dm->gate;
			}
		}
	}
}

void CRhythmCh::NoteOff(UINT8 note)
{
	LastNote = 0xff;
	if (Note[note].device) {
		if (Note[note].ch == 255) { //Internal Rhythm
			Note[note].device->RhythmOff(Note[note].note);
		} else { //Inst Rhythm
			Note[note].device->NoteOff(Note[note].ch);
			Note[note].device->ReleaseCh(Note[note].ch);
		}
		Note[note].device = 0;
		Note[note].ch = 0;
		Note[note].note = 0;
		Note[note].count = 0;
	}
}

void CRhythmCh::NoteOff(CSoundDevice* device, UINT8 ch)
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

void CRhythmCh::SetNRPNRegister(UINT16 reg, UINT16 data)
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

void CRhythmCh::SetRPNRegister(UINT16 reg, UINT16 data)
{
}

UINT16 CRhythmCh::GetNRPNRegister(UINT16 reg)
{
	UINT16 ret = 0;
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

UINT16 CRhythmCh::GetRPNRegister(UINT16 reg)
{
	UINT16 ret = 0;
	return ret;
}

void CRhythmCh::TimerCallBack(UINT32 tick)
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

CThruCh::CThruCh(UINT8 ch, UINT8 och, UINT8 opt, CFITOM* parent) : CMidiCh(ch, parent), OutPort(opt), OutCh(och)
{
	ResetAllCtrl();
	Poly = 0;
	Mono = 0;
	Port = 0;
	PortTime = 0;
}
