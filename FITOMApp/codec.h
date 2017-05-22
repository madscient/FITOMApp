#pragma once
#include	<windows.h>

typedef struct {
	BYTE	bID[4];		// ヘッダ
	DWORD	dSize;			// サイズ
} RIFF_HED;

typedef struct {
	BYTE	bID[4];
	BYTE	bFMT[4];
	DWORD	dChunkSize;
	WORD	wFmt;
	WORD	wChannels;
	DWORD	dRate;
	DWORD	dDataRate;
	WORD	wBlockSize;
	WORD	wSample;
} WAVE_CHUNK;

typedef struct {
	BYTE	bID[4];
	DWORD	dSize;
	BYTE	bData[1];
} DATA_CHUNK;

class AdpcmEncoder
{
public:
	AdpcmEncoder() {};
	~AdpcmEncoder() {};
	BYTE* waveToAdpcm(void *pData, DWORD dSize, DWORD &dAdpcmSize, DWORD rate);
protected:
	RIFF_HED	*m_pRiffHed;
	WAVE_CHUNK	*m_pWaveChunk;
	DATA_CHUNK	*m_pDataChunk;
	short* resampling(DWORD &dSize, DWORD dHiRate);
	virtual int encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize) = 0;
};

class	YmDeltaTEncoder : public AdpcmEncoder {
private:
	static long stepsizeTable[];
public:
	YmDeltaTEncoder();
	~YmDeltaTEncoder();
protected:
	virtual int encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize);
};

class Ym2610AEncoder : public AdpcmEncoder
{
public:
	Ym2610AEncoder();
	~Ym2610AEncoder();
protected:
	virtual int encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize);
private:
	static short step_size[];
	static int step_adj[];
	//buffers
	short* inBuffer;	//temp work buffer, used correct byte order and downsample
	byte* outBuffer;	//output buffer, this is your PCM file, save it

								//decode stuff
	int* jedi_table;
	int acc = 0; //ADPCM accumulator, initial condition must be 0
	int decstep = 0; //ADPCM decoding step, initial condition must be 0

					 //encode stuff
	int diff;
	int step;
	int predsample;
	int index;
	int prevsample = 0; // previous sample, initial condition must be 0
	int previndex = 0; //previous index, initial condition must be 0

	void jedi_table_init();
	byte YM2610_ADPCM_A_Encode(short sample);
	short YM2610_ADPCM_A_Decode(byte code);
	void YM_encode();
};

