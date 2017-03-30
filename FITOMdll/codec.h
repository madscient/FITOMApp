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

class	Adpcm{
private:
	RIFF_HED	*m_pRiffHed;
	WAVE_CHUNK	*m_pWaveChunk;
	DATA_CHUNK	*m_pDataChunk;
public:
	Adpcm();
	~Adpcm();
	BYTE* waveToAdpcm(void *pData, DWORD dSize, DWORD &dAdpcmSize, DWORD dHiRate);
	short* resampling(DWORD &dSize, DWORD dHiRate);
	int encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize);
};
