#pragma once

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

class CFMVoice {
protected:
	FMVOICE	theVoice;
public:
	CFMVoice(FMVOICE& fmv);
	size_t GetName(TCHAR* buf, size_t size);
	uint8_t GetFB();
	uint8_t GetAL();
	int16_t GetLFODepth();
	uint8_t GetLFOFreq();
	uint8_t GetLFOWave();
	uint8_t GetLFODelay();
	uint8_t GetLFORate();
	uint8_t GetAMS();
	uint8_t GetPMS();

	uint8_t GetAR(uint8_t op);
	uint8_t GetDR(uint8_t op);
};
