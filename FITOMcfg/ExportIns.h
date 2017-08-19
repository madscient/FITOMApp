#pragma once

#include <vector>

//Resource code
#define	DEVICE_NONE	0
#define	DEVICE_SSG	1	//YM2149
#define	DEVICE_OPN	2	//YM2203
#define	DEVICE_OPN2	3	//YM2612
#define	DEVICE_OPNA	4	//YM2608
#define	DEVICE_OPM	5	//YM2151
#define	DEVICE_OPLL	6	//YM2413
#define	DEVICE_OPL	7	//YM3526
#define	DEVICE_OPL2	8	//YM3812
#define	DEVICE_OPL3	9	//YMF262
#define DEVICE_OPN3L 10	//YMF288
#define DEVICE_OPNB 11	//YM2610
#define DEVICE_SAA	12	//SAA1099
#define DEVICE_DSG	13	//YM2163
#define	DEVICE_PSG	15	//AY-3-891x
#define	DEVICE_DCSG	16	//SN76489
#define	DEVICE_SCC	17	//SCC with ROM
#define DEVICE_SCCP	18	//SCC for SNATCHER
#define	DEVICE_SSGS	19	//YMZ705
#define	DEVICE_EPSG	20	//AY8930
#define DEVICE_SSGL 21	//YMZ284
#define DEVICE_SSGLP 22	//YMZ294
#define DEVICE_SID	23	//MOS6581/8580
#define	DEVICE_Y8950	24	//YM3801 a.k.a. MSX-AUDIO
#define	DEVICE_OPL3_2	25	//2op ch of OPL3
#define	DEVICE_OPP	26	//YM2164
#define	DEVICE_OPZ	27	//YM2414
#define	DEVICE_OPZ2	28	//YM2424
#define	DEVICE_OPLLP	29	//YMF281
#define	DEVICE_OPLL2	30	//YM2420
#define DEVICE_OPNC 31	//YMF264
#define DEVICE_OPN2C 32	//YM3438
#define DEVICE_OPN2L 33	//YMF276
#define DEVICE_2610B 34	//YM2610B
#define DEVICE_F286 35	//YMF286-K
#define DEVICE_OPN3 36	//YMF297
#define DEVICE_OPN3_L3 37	//OPL mode of YMF297
#define DEVICE_OPN3_N3 38	//OPN mode of YMF297
#define DEVICE_OPLLX	39	//YM2423/MS1823
#define DEVICE_OPK 40	//YM7116
#define DEVICE_OPK2 41	//YM7129
#define DEVICE_OPQ	42	//YM3806/YM3533
#define DEVICE_RYP4	43	//YM2154
#define DEVICE_RYP6	44	//YM3301/YM3302
#define DEVICE_FMS	45	//YMZ735/YMZ736
#define DEVICE_5232	47	//MSM5232
#define DEVICE_PCMD8	65	//YMZ280

#define	DEVICE_ADPCM	119	//virtual device for ADPCM channel
#define	DEVICE_ADPCMA	118	//YM2610/YM2610B
#define	DEVICE_ADPCMB	117	//YM2608/YM2610/YM2610B/YM3801

#define DEVICE_RHYTHM	120	// Virtual device for rhythm channel

#define VOICE_GROUP_NONE 0x00
#define VOICE_GROUP_OPM  0x01
#define VOICE_GROUP_OPNA 0x02
#define VOICE_GROUP_OPL2 0x04
#define VOICE_GROUP_OPL3 0x08
#define VOICE_GROUP_OPLL 0x10
#define VOICE_GROUP_PSG  0x20
#define VOICE_GROUP_OPL4 0x40
#define VOICE_GROUP_PCM	 0x80
#define VOICE_GROUP_MA3  0x100
#define VOICE_GROUP_RHYTHM 0x8000
#define VOICE_GROUP_ALL  0xffff

struct BankNames{
	int group;
	int bank;
	TCHAR name[64];
};
struct DEVIDMAP {
	const UINT32 devid;
	const UINT32 voicegroup;
	const TCHAR* chipname;
	const TCHAR* chipcode;
};

typedef std::vector<BankNames> BankNamesArray;
typedef std::vector<DEVIDMAP> DevMapArray;

void WritePcmPatchNameBank(CStdioFile& dst, TCHAR* bnkname, LPCTSTR groupname, LPCTSTR bnkfile);
void WritePatchNameBank(CStdioFile& dst, TCHAR* bnkname, LPCTSTR groupname, LPCTSTR bnkfile);
void WritePatchNameSection(CStdioFile& dst, DevMapArray& devuse, BankNamesArray& arry);
void WriteNoteNameBank(CStdioFile& dst, TCHAR* bnkname, LPCTSTR bnkfile);
void WriteNoteNameSection(CStdioFile& dst, BankNamesArray& arry);
void WriteInstrumentSection(CStdioFile& dst, DevMapArray& devuse, BankNamesArray& arry);
void GetDevUse(DevMapArray& devuse);
void ExportInsFile();
void AddDevMapArray(DevMapArray& devuse, DEVIDMAP& devadd);
