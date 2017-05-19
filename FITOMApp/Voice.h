#pragma once
class CVoice
{
public:
	enum VoiceType {
		none, FM, PSG, PCM, Rhythm, Perf,
	};
protected:
	TCHAR name[32];
	UINT32 id;
	UINT32 voiceType;
	UINT32 chipType;
public:
	CVoice();
	~CVoice();
};

class CLFOParam
{
	UINT8 waveform;
	UINT8 freq;
	UINT16 rate;
	UINT16 delay;
	SINT16 depth;
};

class COPParam
{
	UINT8 AR;
	UINT8 DR;
	UINT8 SR;
	UINT8 SL;
	UINT8 RR;
	UINT8 EGBias;
	UINT8 EGType;
	UINT8 Reverb;
	UINT8 KSR;
	UINT8 KSL;
	UINT8 TL;
	UINT8 AME;
	UINT8 PME;
	UINT8 WS;
	UINT8 generic1;
	UINT8 generic2;
	UINT8 FIX;
	UINT8 ML;
	UINT8 DT1;
	UINT8 DT2;
	UINT16 FixedFreq;
	UINT16 EnvFreq;
	CLFOParam levelLFO;
};

class CFMVoice : public CVoice
{
	UINT16 AL;
	UINT16 FB;
	UINT16 NFREQ;
	SINT16 PseudoDT;
	CLFOParam pitchLFO;
	COPParam op[4];
};

