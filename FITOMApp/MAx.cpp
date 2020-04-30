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
	: CSoundDevice(DEVICE_SD1, 16, fsamp, 256, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x80), PresetIndex(0)
{
	for (int i = 0; i < 16; i++) { Instrument[i] = 0xff; }
}

void CSD1::Init()
{
	port->reset();
	::Sleep(10);
	//port->read(0x04);
	port->write(0x1D, 0);	//OUTPUT Power(5V)
	port->write(0x02, 0x0E);	//VREF Power ON
	port->flush();
	::Sleep(1);
	port->write(0x00, 0x01);	//CLKEN
	port->write(0x01, 0x00);	//AKRST
	port->write(0x1A, 0xA3);
	port->flush();
	::Sleep(1);
	port->write(0x1A, 0x00);
	port->flush();
	::Sleep(30);
	port->write(0x02, 0x00);	//Analog Power ON
	port->flush();
	//add
	port->write(0x19, 0x81);	//MASTER VOL (-18dB)
	port->write(0x1B, 0x3F);	//interpolation
	port->write(0x14, 0x00);	//interpolation
	port->write(0x03, 0x01);	//Analog Gain
	port->flush();

	port->write(0x08, 0xF6);
	port->flush();
	::Sleep(21);
	port->write(0x08, 0x00);
	port->write(0x09, 0xcc);	//sequencer volume (0dB)
	port->write(0x0A, 0x00);
	port->flush();

	port->write(0x17, 0x40);//MS_S
	port->write(0x18, 0x00);
	port->flush();
}

void CSD1::SetVoice(uint8_t ch, FMVOICE* voice, int update)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		bool changed = attr->SetVoice(voice);
		if (1 || changed) {
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
	port->write(0x08, 0x16);	//Sequencer Reset
	BYTE tonebuf[1 + 1 + 480 + 4];
	int idx = 0;
	tonebuf[idx++] = 0x07;	//Register Address
	tonebuf[idx++] = 0x90;	//Header: Tone table size (always 16)
	for (int i = 0; i < 16; i++) {
		tonebuf[idx++] = PresetTone[i].APS & 7;	//1-0:Basic Octave
		tonebuf[idx++] = ((PresetTone[i].APS << 2) & 0xc0) | (PresetTone[i].AL & 0x7);	//7-6:LFO|2-0:AL
		for (int j = 0; j < 4; j++) {
			BYTE tmp;
			tmp = (PresetTone[i].op[j].RR == 0) ? 0x04 : 0;	//SUS
			tonebuf[idx++] = ((PresetTone[i].op[j].SR << 1) & 0xf0) | ((PresetTone[i].op[j].KS & 2) >> 1) | tmp;	//[+0]7-4:SR|3:SUS|0:KSR
			tonebuf[idx++] = ((PresetTone[i].op[j].RR << 1) & 0xf0) | (PresetTone[i].op[j].DR >> 3);	//[+1]7-4:RR|3-0:DR
			tonebuf[idx++] = ((PresetTone[i].op[j].AR << 1) & 0xf0) | (PresetTone[i].op[j].SL >> 3);	//[+2]7-4:AR|3-0:SL
			tonebuf[idx++] = ((PresetTone[i].op[j].TL << 1) & 0xfc) | (PresetTone[i].op[j].KS >> 4);	//[+3]7-2:TL|1-0:KSL
			tonebuf[idx++] = PresetTone[i].op[j].AVF;	//[+4]6-5:DAM|4:EAM|2-1:DVB|0:EVB
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
	port->write(0x08, 0x00);	//Sequencer reset clear
	port->writeBurst(tonebuf, idx);
	port->flush();
}

void CSD1::UpdateVoice(uint8_t ch)
{
	if (Instrument[ch] > 15) {
		SetVoice(ch, GetChAttribute(ch)->GetVoice(), 1);
	}
	port->write(0x0b, ch);
	port->write(0x0f, (GetChAttribute(ch)->IsRunning() ? 0x40 : 0) | Instrument[ch] & 0xf);
}

void CSD1::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	uint8_t evol = CalcVolExpVel(127, attr->GetExpress(), attr->velocity);
	evol = 31 - Linear2dB(evol, RANGE48DB, STEP075DB, 5);
	uint8_t cvol = 31 - Linear2dB(attr->GetVolume(), RANGE48DB, STEP075DB, 5);
	port->write(0x0b, ch);
	port->write(0x0c, evol << 2);
	port->write(0x10, (cvol << 2) | 1);
}

void CSD1::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	port->write(0x0b, ch);
	port->write(0x0d, ((fnum->fnum >> 5) & 0x78) | fnum->block);
	port->write(0x0e, (fnum->fnum >> 1) & 0x7f);
	port->write(0x11, 0x00);// XVB
	port->write(0x12, 0x08);// FRAC
	port->write(0x13, 0x00);// FRAC 
}

void CSD1::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
}

void CSD1::UpdateKey(uint8_t ch, uint8_t keyon)
{
	port->write(0x0b, ch);
	port->write(0x0f, (Instrument[ch] & 0xf) | (keyon ? 0x40 : 0));
}

void CSD1::ResetChannel(uint8_t ch)
{
	port->write(0x0b, ch);
	port->write(0x0f, 0x30);
}

void CSD1::register_dump()
{
#if 0
	for (int i = 0; i < 16; i++) {
		fprintf(stderr, _T("\n%04X: "), i);
		for (int j = 0; j < 16; j++) {
			port->write(21, i * 16 + j);	//reg addr
			uint8_t data = port->read(22);
			fprintf(stderr, _T("%02X "), data);
		}
	}
	fprintf(stderr, _T("\n"));
#endif
}