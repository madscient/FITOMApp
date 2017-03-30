#include "STDAFX.H"
#include "SoundDev.h"

namespace ROM {
static SINT8 sintable[] = {
	0,3,6,9,13,16,19,22,25,28,31,34,37,40,43,
	46,49,52,54,57,60,63,65,68,71,73,76,78,80,83,
	85,87,89,91,93,95,97,99,101,102,104,105,107,108,110,
	111,112,113,114,115,116,117,117,118,119,119,119,120,120,120,
	120,120,120,120,119,119,119,118,117,117,116,115,114,113,112,
	111,110,108,107,105,104,102,101,99,97,95,93,91,89,87,
	85,83,80,78,76,73,71,68,65,63,60,57,54,52,49,
	46,43,40,37,34,31,28,25,22,19,16,13,9,6,3,
	0,-3,-6,-9,-13,-16,-19,-22,-25,-28,-31,-34,-37,-40,-43,
	-46,-49,-52,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-80,-83,
	-85,-87,-89,-91,-93,-95,-97,-99,-101,-102,-104,-105,-107,-108,-110,
	-111,-112,-113,-114,-115,-116,-117,-117,-118,-119,-119,-119,-120,-120,-120,
	-120,-120,-120,-120,-119,-119,-119,-118,-117,-117,-116,-115,-114,-113,-112,
	-111,-110,-108,-107,-105,-104,-102,-101,-99,-97,-95,-93,-91,-89,-87,
	-85,-83,-80,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-52,-49,
	-46,-43,-40,-37,-34,-31,-28,-25,-22,-19,-16,-13,-9,-6,-3,
};

static UINT8 speedstep[] = {
	1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 16, 20, 24, 30, 40, 48, 60, 80, 120
};

extern const UINT8 VolCurveLin[] = {
	127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,
	111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,
	95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,
	79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,
	63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,
	47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
	31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,
	15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
};

extern const UINT8 VolCurveInv[] = {
	127,121,115,104,96,90,85,80,77,73,71,68,66,63,61,59,
	58,56,54,53,51,50,49,48,46,45,44,43,42,41,40,39,
	39,38,37,36,35,35,34,33,32,32,31,30,30,29,29,28,
	27,27,26,26,25,25,24,24,23,23,22,22,21,21,20,20,
	19,19,19,18,18,17,17,17,16,16,15,15,15,14,14,14,
	13,13,13,12,12,12,11,11,11,10,10,10,9,9,9,9,
	8,8,8,7,7,7,7,6,6,6,5,5,5,5,4,4,
	4,4,3,3,3,3,3,2,2,2,2,1,1,1,1,0,
};

};

#define ABSAMP	120
#define TABLEN	240

extern SINT8 GetLFOWave(UINT8 waveform, UINT8 speed, UINT16 phase)
{
	signed int ret = 0;
	unsigned short spp = (phase % TABLEN) * ROM::speedstep[speed%sizeof(ROM::speedstep)];
	unsigned short ph = spp % TABLEN;
	unsigned short pl = spp / TABLEN;
	switch (waveform) {
	case 6://sin wave
		ret = ROM::sintable[ph];
		break;
	case 0://saw wave
		ret = (ph < (TABLEN/2)) ? ph : (ph - (ABSAMP*2));
		break;
	case 2://triangular
		if (0 <= ph && ph <(TABLEN/4)) ret = ph << 1;
		else if ((TABLEN/4) <= ph && ph < ((TABLEN/4)*3)) ret = (ABSAMP*2) - (ph<<1);
		else if (((TABLEN/4)*3) <= ph && ph < TABLEN) ret = ((ph-((TABLEN/4)*3))<<1) - ABSAMP;
		break;
	case 1://square
		ret = (ph < ABSAMP) ? ABSAMP : (-ABSAMP);
		break;
	case 3://sample & hold
		if (!(spp & 0xff)) {
			ret = 0;
		} else {
			char lfsr = (spp & 0xff);
			lfsr = (lfsr >> 1) ^ (-(char)(lfsr & 1u) & 0xb9);
			ret = (int)lfsr;
		}
		break;
	case 4://one shot saw
		ret = (((phase*ROM::speedstep[speed%sizeof(ROM::speedstep)]) < (TABLEN / 2))) ? ph : ABSAMP;
		break;
	case 5://one shot delta
		if (phase < UINT16(TABLEN/2)) {
			if (0 <= ph && ph <(TABLEN/4)) ret = ph << 1;
			else if ((TABLEN/4) <= ph && ph < (TABLEN/2)) ret = (ABSAMP*2) - (ph<<1);
		}
		break;
	}
	return ret;
}

extern UINT8 CalcVolExpVel(UINT8 vol, UINT8 exp, UINT8 vel)
{
	UINT16 evol = vol;
#if 1
	evol = (evol * exp + evol + exp) >> 7;
	evol = (evol * vel + evol + vel) >> 7;
#else
	evol = (UINT16(vol) + UINT16(exp) + UINT16(vel)) / 3;
#endif
	return (UINT8)evol;
}

extern UINT8 CalcEffectiveLevel(UINT8 vev, UINT8 tl)
{
	tl = 127 - tl;
#if 0
	UINT16 evol = (vev * tl + vev + tl) >> 7;
#else
	UINT16 evol = (UINT16(vev) + UINT16(tl)) >> 1;
#endif
	return ROM::VolCurveLin[UINT8(evol)];
}

ISoundDevice::CEnvelope::CEnvelope() : phase(EG_NONE), value(0), count(0), param(0)
{
}

void ISoundDevice::CEnvelope::Start(const FMOP* env)
{
	param = env;
	phase = EG_ATTACK;
	value = param->EGS;
	count = 0;
}

void ISoundDevice::CEnvelope::Stop()
{
	phase = EG_NONE;
	value = 0;
	count = 0;
	param = NULL;
}

void ISoundDevice::CEnvelope::Release()
{
	if (phase != EG_NONE) {
		phase = EG_RELEASE;
		count = 0;
	}
}

int ISoundDevice::CEnvelope::Update()
{
	if (!param) return 0;
	int ret = 1;
	SINT16 tmp = value;
	switch (phase) {
	case EG_ATTACK:
		tmp += param->AR;
		if (tmp < 127) {
			value = tmp;
			count++;
		} else {
			phase = EG_DECAY;
			value = 127;
			count = 0;
		}
		break;
	case EG_DECAY:
		tmp -= param->DR;
		if (tmp > 0 && tmp > (127-param->SL)) {
			value = tmp;
			count++;
		} else {
			phase = EG_SUSTAIN;
			value = 127 - param->SL;
			count = 0;
		}
		break;
	case EG_SUSTAIN:
		tmp -= (param->SR >> 3);
		if (tmp > 0) {
			value = tmp;
		} else {
			value = 0;
		}
		count++;
		break;
	case EG_RELEASE:
		if (tmp <= 0) {
			Stop();
		} else {
			tmp -= param->RR;
			if (tmp > 0) {
				value = tmp;
			} else {
				value = 0;
			} 
			count++;
		}
		break;
	case EG_NONE:
	default:
		ret = 0;
		break;
	}
	return ret;
}

ISoundDevice::CLFOControl::CLFOControl()
	: count(0), value(0), delay(0), rate(0), status(LFO_NONE)
{
}

void ISoundDevice::CLFOControl::Start(UINT8 dly, UINT8 rt)
{
	delay = UINT16(dly) << 2;
	rate = rt;
	status = LFO_DELAYING;
	value = 0;
	count = 0;
}

void ISoundDevice::CLFOControl::SetParam(UINT8 dly, UINT8 rt)
{
	delay = UINT16(dly) << 2;
	rate = rt;
}

void ISoundDevice::CLFOControl::Stop()
{
	status = LFO_NONE;
	value = 0;
	count = 0;
}

int ISoundDevice::CLFOControl::Update()
{
	int ret = 1;
	if (status != LFO_NONE && count < 65534) { count++; }
	switch (status) {
	case LFO_DELAYING:
		if (count > delay) {
			status = LFO_INCREASING;
			count = 0;
			//Fall Through
		} else {
			break;
		}
	case LFO_INCREASING:
		value += rate;
		if (value > 127) {
			value = 127;
			status = LFO_RETAINING;
		}
		//Fall Through
	case LFO_RETAINING:
		break;
	case LFO_NONE:
	default:
		ret = 0;
		break;
	}
	return ret;
}
