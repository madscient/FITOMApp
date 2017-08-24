#include "stdafx.h"
#include "MAx.h"

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)

CSD1::CSD1(CPort* pt, int fsamp)
	: CSoundDevice(DEVICE_SD1, 16, fsamp, 128, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x80), PresetIndex(0)
{
	for (int i = 0; i < 16; i++) { Instrument[i] = 0; }
	port->reset();
	::Sleep(1);
	port->write(0x1D, 1);	//OUTPUT Power(3V3)
	port->write(0x02, 0x0E);	//VREF Power ON
	::Sleep(1);
	port->write(0x00, 0x01);	//CLKEN
	port->write(0x01, 0x00);	//AKRST
	port->write(0x1A, 0xA3);
	::Sleep(1);
	port->write(0x1A, 0x00);
	::Sleep(30);
	port->write(0x02, 0x00);	//Analog Power ON
	//add
	port->write(0x19, 0x20);	//MASTER VOL
	port->write(0x1B, 0x3F);	//interpolation
	port->write(0x14, 0x00);	//interpolation
	port->write(0x03, 0x01);	//Analog Gain

	port->write(0x08, 0xF6);
	::Sleep(21);
	port->write(0x08, 0x00);
	port->write(0x09, 0xF8);
	port->write(0x0A, 0x00);

	port->write(0x17, 0x40);//MS_S
	port->write(0x18, 0x00);
}

void CSD1::SetVoice(UINT8 ch, FMVOICE* voice, int update)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		BOOL changed = attr->SetVoice(voice);
		if (changed) {
			int inst = -1;
			for (int i = 0; i < 16; i++) {
				if (memcmp(voice, &PresetTone[i], sizeof(FMVOICE)) == 0) {
					inst = i;
					break;
				}
			}
			if (inst == -1 && update) {	//Penalty
				inst = PresetIndex;
				memcpy(&PresetTone[PresetIndex], voice, sizeof(FMVOICE));
				PresetIndex = (PresetIndex + 1) & 0xf;
				UpdatePresetTone();
			}
			if (inst >= 0) {
				Instrument[ch] = inst;
				UpdateVoice(ch);
			}
		}
	}
}

void CSD1::UpdatePresetTone()
{
	port->write(0x08, 0xF6);	//Sequencer Reset
	BYTE tonebuf[1 + 480 + 4];
	int idx = 0;
	tonebuf[idx++] = 0x90;	//Header: Tone table size (always 16)
	for (int i = 0; i < 16; i++) {
		tonebuf[idx++] = 0x01;	//1-0:Basic Octave
		tonebuf[idx++] = (PresetTone[i].AMS << 6) | (PresetTone[i].AL & 0x7);	//7-6:LFO|2-0:AL
		for (int j = 0; j < 4; j++) {
			BYTE tmp;
			tmp = (PresetTone[i].op[j].RR == 0) ? 0x04 : 0;	//SUS
			tonebuf[idx++] = ((PresetTone[i].op[j].SR << 1) & 0xf0) | (PresetTone[i].op[j].KSR & 1) | tmp;	//[+0]7-4:SR|3:SUS|0:KSR
			tonebuf[idx++] = ((PresetTone[i].op[j].RR << 1) & 0xf0) | (PresetTone[i].op[j].DR >> 3);	//[+1]7-4:RR|3-0:DR
			tonebuf[idx++] = ((PresetTone[i].op[j].AR << 1) & 0xf0) | (PresetTone[i].op[j].SL >> 3);	//[+2]7-4:AR|3-0:SL
			tonebuf[idx++] = ((PresetTone[i].op[j].TL << 1) & 0xfc) | (PresetTone[i].op[j].KSL & 3);	//[+3]7-2:TL|1-0:KSL
			tmp = ((PresetTone[i].op[j].VIB & 1) | ((PresetTone[i].op[j].AM & 1) << 4)) << 2;
			tonebuf[idx++] = ((PresetTone[i].op[j].AM & 0x6) << 3) | ((PresetTone[i].op[j].VIB & 0x6) >> 1) | tmp;	//[+4]6-5:DAM|4:EAM|2-1:DVB|0:EVB
			tonebuf[idx++] = ((PresetTone[i].op[j].MUL << 4) & 0xf0) | (PresetTone[i].op[j].DT1 & 7);	//[+5]7-4:MUL|2-0:DT1
			switch (j) {
			case 0: tmp = PresetTone[i].FB & 0x7; break;
			case 2: tmp = (PresetTone[i].FB >> 3) & 0x7; break;
			default: tmp = 0;
			}
			tonebuf[idx++] = ((PresetTone[i].op[j].WS << 3) & 0xf8) | tmp;	//[+6]7-3:WS|2-0:FB
		}
	}
	tonebuf[idx++] = 0x80;	//Footer
	tonebuf[idx++] = 0x03;	//Footer
	tonebuf[idx++] = 0x81;	//Footer
	tonebuf[idx++] = 0x80;	//Footer
	port->write(0x08, 0x00);	//Sequencer Start
	port->writeBurst(0x07, tonebuf, idx);
}

void CSD1::UpdateVoice(UINT8 ch)
{
	SetReg(0x0b, ch);
	SetReg(0x0f, Instrument[ch] & 0xf);
}

void CSD1::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = CalcVolExpVel(127, attr->express, attr->velocity);
	evol = Linear2dB(evol, RANGE48DB, STEP075DB, 5);
	UINT8 cvol = Linear2dB(attr->volume, RANGE48DB, STEP075DB, 5);
	SetReg(0x0b, ch);
	SetReg(0x0d, evol << 2);
	SetReg(0x10, (cvol << 2) | 1);
}

void CSD1::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x0b, ch);
	SetReg(0x0d, ((fnum->fnum >> 5) & 0x78) | fnum->block);
	SetReg(0x0e, (fnum->fnum >> 1) & 0x7f);
}

void CSD1::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
}

void CSD1::UpdateKey(UINT8 ch, UINT8 keyon)
{
	SetReg(0x0b, ch);
	SetReg(0x0f, (GetReg(0xf, 0) & 0x3f) | (keyon ? 0x40 : 0));
}