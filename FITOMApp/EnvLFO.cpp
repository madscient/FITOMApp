#include "STDAFX.H"
#include "SoundDev.h"
#include <math.h>

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

/*
extern const UINT8 VolCurveLin[] = {	//0.75dB step;127=-96dB
	127,56,48,43,40,37,35,34,32,31,29,28,27,26,26,25,
	24,23,23,22,21,21,20,20,19,19,18,18,18,17,17,16,
	16,16,15,15,15,14,14,14,13,13,13,13,12,12,12,12,
	11,11,11,11,10,10,10,10,9,9,9,9,9,8,8,8,
	8,8,8,7,7,7,7,7,7,6,6,6,6,6,6,5,
	5,5,5,5,5,5,5,4,4,4,4,4,4,4,3,3,
	3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
};
*/
extern const double GM2dB[] = {
	-96.0,
	-84.15214884,
	-72.11094901,
	-65.06729865,
	-60.06974919,
	-56.19334866,
	-53.02609882,
	-50.34822724,
	-48.02854936,
	-45.98244846,
	-44.15214884,
	-42.49644143,
	-40.984899,
	-39.59441475,
	-38.30702741,
	-37.10849848,
	-35.98734953,
	-34.93419198,
	-33.94124863,
	-33.0020048,
	-32.11094901,
	-31.26337705,
	-30.45524161,
	-29.6830354,
	-28.94369917,
	-28.23454849,
	-27.55321492,
	-26.89759827,
	-26.26582758,
	-25.65622892,
	-25.06729865,
	-24.49768108,
	-23.94614971,
	-23.41159124,
	-22.89299216,
	-22.38942706,
	-21.90004881,
	-21.42407988,
	-20.96080497,
	-20.50956456,
	-20.06974919,
	-19.64079457,
	-19.22217722,
	-18.81341062,
	-18.41404178,
	-18.02364829,
	-17.64183557,
	-17.26823452,
	-16.90249934,
	-16.54430564,
	-16.19334866,
	-15.84934179,
	-15.51201509,
	-15.18111405,
	-14.85639845,
	-14.53764126,
	-14.22462776,
	-13.91715461,
	-13.6150291,
	-13.31806837,
	-13.02609882,
	-12.73895544,
	-12.45648126,
	-12.17852686,
	-11.90494988,
	-11.63561457,
	-11.37039142,
	-11.10915673,
	-10.85179233,
	-10.59818521,
	-10.34822724,
	-10.10181489,
	-9.858848981,
	-9.619234433,
	-9.382880049,
	-9.149698303,
	-8.919605147,
	-8.692519831,
	-8.468364731,
	-8.247065187,
	-8.028549359,
	-7.812748083,
	-7.599594743,
	-7.389025143,
	-7.180977396,
	-6.97539181,
	-6.772210788,
	-6.571378733,
	-6.372841952,
	-6.176548572,
	-5.982448461,
	-5.790493145,
	-5.600635744,
	-5.412830896,
	-5.227034694,
	-5.043204627,
	-4.861299517,
	-4.681279468,
	-4.503105811,
	-4.326741054,
	-4.152148838,
	-3.979293887,
	-3.808141968,
	-3.63865985,
	-3.470815266,
	-3.304576875,
	-3.139914228,
	-2.976797731,
	-2.815198619,
	-2.655088921,
	-2.496441432,
	-2.339229687,
	-2.183427931,
	-2.029011099,
	-1.875954785,
	-1.724235224,
	-1.573829269,
	-1.424714368,
	-1.276868546,
	-1.130270383,
	-0.984898996,
	-0.840734026,
	-0.697755611,
	-0.555944381,
	-0.415281432,
	-0.275748318,
	-0.137327034,
	0,
};

extern const UINT8 VolCurveLin[] = {	//0.75dB step;127=-96dB
	127, 112, 96, 86, 80, 74, 70, 67, 64, 61, 58, 56, 54, 52, 51, 49,
	47, 46, 45, 44, 42, 41, 40, 39, 38, 37, 36, 35, 35, 34, 33, 32,
	31, 31, 30, 29, 29, 28, 27, 27, 26, 26, 25, 25, 24, 24, 23, 23,
	22, 22, 21, 21, 20, 20, 19, 19, 18, 18, 18, 17, 17, 16, 16, 16,
	15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10,
	10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6,
	6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
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
			if (0 <= ph && ph < (TABLEN / 4)) {
				ret = ph << 1;
			}
			else if ((TABLEN / 4) <= ph && ph < (TABLEN / 2)) {
				ret = (ABSAMP * 2) - (ph << 1);
			}
		}
		break;
	}
	return ret;
}

extern UINT8 CalcVolExpVel(int vol, int exp, int vel)
{
	double dvol = ROM::GM2dB[vol];
	double dexp = ROM::GM2dB[exp];
	double dvel = ROM::GM2dB[vel];
	double deff = 127.0 * pow(10, (dvol + dexp + dvel) / 40.0);

	int evol = round(deff);
	evol = max(0, evol);
	evol = min(127, evol);
	return (UINT8)evol;
}

extern UINT8 CalcEffectiveLevel(UINT8 vev, UINT8 tl)
{
	tl = 127 - tl;
	UINT16 evol = (UINT16(vev) + UINT16(tl)) >> 1;
	return UINT8(127-evol);
}

extern UINT8 CalcLinearLevel(UINT8 vev, UINT8 tl)
{
	double dvev = ROM::GM2dB[vev];
	double dtl = double(tl) * (-0.75);
	double deff = 127.0 * pow(10, (dvev + dtl) / 40.0);
	int evol = round(deff);
	evol = max(0, evol);
	evol = min(127, evol);
	return (127-evol);
}

extern UINT8 Linear2dB(UINT8 evol, int range, int step, int bw)
{
	evol = evol & step;
	UINT8 ret;
	UINT8 lim = (127 >> range);
	if (evol == 0) {
		ret = lim;
	}
	else {
		ret = ROM::VolCurveLin[UINT8(evol)];
		ret = min(ret, lim - 1);
	}
	ret >>= 7 - range - bw;
	return ret;

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
		if ((count & 0x1) == 1) {	//ディケイフェーズは1/2
			tmp -= param->DR;
			if (tmp > 0 && tmp > (127 - param->SL)) {
				value = tmp;
				count++;
			}
			else {
				phase = EG_SUSTAIN;
				value = 127 - param->SL;
				count = 0;
			}
		}
		else {
			count++;
		}
		break;
	case EG_SUSTAIN:
		if ((count & 0x7) == 7) {	//サスティンフェーズは1/8
			tmp -= param->SR;
			if (tmp > 0) {
				value = tmp;
			}
			else {
				value = 0;
			}
		}
		count++;
		break;
	case EG_RELEASE:
		if ((count & 0x3) == 3) {	//リリースフェーズは1/4
			if (tmp <= 0) {
				Stop();
			}
			else {
				tmp -= param->RR;
				if (tmp > 0) {
					value = tmp;
				}
				else {
					value = 0;
				}
				count++;
			}
		}
		else {
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
