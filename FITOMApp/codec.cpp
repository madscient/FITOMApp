#include "stdafx.h"
#include "codec.h"
#include <math.h>

// コンストラクタ
YmDeltaTEncoder::YmDeltaTEncoder(){
}

// デストラクタ
YmDeltaTEncoder::~YmDeltaTEncoder(){
}

// ADPCM変換
BYTE* AdpcmEncoder::waveToAdpcm(void *pData, DWORD dSize, DWORD &dAdpcmSize, DWORD rate){
	// RIFFヘッダ確認
	m_pRiffHed = reinterpret_cast<RIFF_HED*>(pData);
	// ヘッダチェック
	if (m_pRiffHed->bID[0] != 'R' && m_pRiffHed->bID[1] != 'I' && m_pRiffHed->bID[2] != 'F' && m_pRiffHed->bID[3] != 'F'){
		return NULL;
	}
	// WAVEヘッダチェック
	m_pWaveChunk = reinterpret_cast<WAVE_CHUNK*>(static_cast<BYTE*>(pData)+0x00000008);
	if (m_pWaveChunk->bID[0] != 'W' && m_pWaveChunk->bID[1] != 'A' && m_pWaveChunk->bID[2] != 'V' && m_pWaveChunk->bID[3] != 'E'){
		return NULL;
	}
	// フォーマットヘッダチェック
	if (m_pWaveChunk->bFMT[0] != 'f' && m_pWaveChunk->bFMT[1] != 'm' && m_pWaveChunk->bFMT[2] != 't' && m_pWaveChunk->bFMT[3] != ' '){
		return NULL;
	}
	// リニアPCMチェック
	if (m_pWaveChunk->wFmt != 0x0001){
		// リニアPCM以外はNGにする
		return NULL;
	}
	m_pDataChunk = reinterpret_cast<DATA_CHUNK*>(reinterpret_cast<BYTE*>(&m_pWaveChunk->wFmt) + m_pWaveChunk->dChunkSize);
	// waveデータリサンプリング
	DWORD	dPcmSize;
	short	*pPcm = resampling(dPcmSize, rate);
	// リサンプリングできなかった場合
	if (pPcm == NULL){
		return NULL;
	}

	// adpcm変換
	BYTE	*pAdpcm = new BYTE[dPcmSize / 2];
	encode(pPcm, pAdpcm, dPcmSize);
	dAdpcmSize = dPcmSize / 2;
	delete[] pPcm;
	return	pAdpcm;
}

// リサンプリング
short* AdpcmEncoder::resampling(DWORD &dSize, DWORD rate){
	// フォーマットチェック（16bit以外だったらＮＧ）
	if (m_pWaveChunk->wSample != 16){
		return NULL;
	}
	// モノラル化
	short *pPcm;
	int		iPcmSize;
	if (m_pWaveChunk->wChannels == 2){
		iPcmSize = static_cast<int>(m_pDataChunk->dSize / 4);
		pPcm = new short[iPcmSize];		// 半分のサイズになる
		short	*pSrc = reinterpret_cast<short*>(&m_pDataChunk->bData[0]);
		short	*pDis = pPcm;
		for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
			int	iPcm = *pSrc++;	// Ｌ
			iPcm += *pSrc++;	// Ｒ
			iPcm /= 2;
			*pDis++ = static_cast<short>(iPcm);
		}
	}
	else if (m_pWaveChunk->wChannels == 1){
		iPcmSize = static_cast<int>(m_pDataChunk->dSize / 2);
		pPcm = new short[iPcmSize];
		memcpy(pPcm, &m_pDataChunk->bData[0], m_pDataChunk->dSize);
	}
	else{
		return NULL;
	}
	// リサンプリング
	int iSrcRate = static_cast<int>(m_pWaveChunk->dRate);	// waveのレート
	int iDisRate = rate;	// 出力のサンプルレート
	int iDiff = 0;
	int	iSampleSize = 0;
	// リサンプリング後のファイルサイズを算出
	for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
		iDiff += iDisRate;
		while (iDiff >= iSrcRate){
			// 書き出し
			iSampleSize++;
			iDiff -= iSrcRate;
		}
	}
	if (iDiff > 0) iSampleSize++;
	// リサンプリング後のバッファを作成する
	int iResampleBuffSize = iSampleSize;
	if (iSampleSize % 64 > 0) iResampleBuffSize += (64 - (iSampleSize % 64));
	short *pResampleBuff = new short[iResampleBuffSize];
	ZeroMemory(pResampleBuff, sizeof(short) * iResampleBuffSize);
	// リサンプリング処理を実施する
	short iSampleCnt = 0;
	int iSmple = 0;
	iDiff = 0;
	iSmple = 0;
	short *pResampleDis = pResampleBuff;
	// リサンプリング後のファイルサイズを算出
	BOOL	bUpdate = FALSE;
	for (int iCnt = 0; iCnt < iPcmSize; iCnt++){
		iSmple += static_cast<int>(pPcm[iCnt]);		// サンプルを加算する
		iSampleCnt++;
		iDiff += iDisRate;
		bUpdate = FALSE;
		while (iDiff >= iSrcRate){
			*pResampleDis++ = static_cast<short>(iSmple / iSampleCnt);
			// 書き出し
			iDiff -= iSrcRate;
			bUpdate = TRUE;
		}
		if (bUpdate == TRUE){
			iSampleCnt = 0;
			iSmple = 0;
			bUpdate = FALSE;
		}
	}
	if (iSampleCnt > 0){
		*pResampleDis++ = static_cast<short>(iSmple / iSampleCnt);
	}
	// ソースPCMバッファを破棄する
	delete[] pPcm;
	dSize = iResampleBuffSize;
	return	pResampleBuff;
}

// エンコード
long YmDeltaTEncoder::stepsizeTable[16] =
{
	57, 57, 57, 57, 77, 102, 128, 153,
	57, 57, 57, 57, 77, 102, 128, 153
};

int YmDeltaTEncoder::encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize){
	int iCnt;
	long i, dn, xn, stepSize;
	unsigned char adpcm;
	unsigned char adpcmPack;

	// 初期値設定
	xn = 0;
	stepSize = 127;

	for (iCnt = 0; iCnt < static_cast<int>(iSampleSize); iCnt++){
		// エンコード処理
		dn = *pSrc - xn;		// 差分抽出
		pSrc++;
		i = (abs(dn) << 16) / (stepSize << 14);
		if (i > 7){
			i = 7;
		}
		adpcm = (unsigned char)i;
		i = (adpcm * 2 + 1) * stepSize >> 3;
		if (dn < 0){
			adpcm |= 0x8;
			xn -= i;
		}
		else{
			xn += i;
		}
		stepSize = (stepsizeTable[adpcm] * stepSize) / 64;
		if (stepSize < 127){
			stepSize = 127;
		}
		else if (stepSize > 24576){
			stepSize = 24576;
		}
		// ADPCMデータ合成
		if ((iCnt & 0x01) == 0){
			adpcmPack = (adpcm << 4);
		}
		else{
			adpcmPack |= adpcm;
			*pDis = adpcmPack;
			pDis++;
		}
	}
	return 0;
}


///
short Ym2610AEncoder::step_size[] = {
	16, 17, 19, 21, 23, 25, 28, 31, 34, 37,
	41, 45, 50, 55, 60, 66, 73, 80, 88, 97,
	107, 118, 130, 143, 157, 173, 190, 209, 230, 253,
	279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552
}; //49 items
int Ym2610AEncoder::step_adj[] = { -1, -1, -1, -1, 2, 5, 7, 9, -1, -1, -1, -1, 2, 5, 7, 9 };

Ym2610AEncoder::Ym2610AEncoder()
{
	jedi_table_init();
}

Ym2610AEncoder::~Ym2610AEncoder()
{
}


				   //jedi table is used speed up decoding, run this to init the table before encoding. Mame copy-pasta.
void Ym2610AEncoder::jedi_table_init()
{
	int step, nib;

	jedi_table = new int[16 * 49];
	for (step = 0; step < 49; step++)
	{
		for (nib = 0; nib < 16; nib++)
		{
			int value = (2 * (nib & 0x07) + 1) * step_size[step] / 8;
			jedi_table[step * 16 + nib] = ((nib & 0x08) != 0) ? -value : value;
		}
	}
}

//decode sub, returns decoded 12bit data
short Ym2610AEncoder::YM2610_ADPCM_A_Decode(byte code)
{
	acc += jedi_table[decstep + code];
	if ((acc & ~0x7ff) != 0) // acc is > 2047
		acc |= ~0xfff;
	else acc &= 0xfff;
	decstep += step_adj[code & 7] * 16;
	if (decstep < 0) decstep = 0;
	if (decstep > 48 * 16) decstep = 48 * 16;
	return (short)acc;
}

// our encoding sub, returns ADPCM nibble
byte Ym2610AEncoder::YM2610_ADPCM_A_Encode(short sample)
{
	int tempstep;
	byte code;

	predsample = prevsample;
	index = previndex;
	step = step_size[index];

	diff = sample - predsample;
	if (diff >= 0)
		code = 0;
	else
	{
		code = 8;
		diff = -diff;
	}

	tempstep = step;
	if (diff >= tempstep)
	{
		code |= 4;
		diff -= tempstep;
	}
	tempstep >>= 1;
	if (diff >= tempstep)
	{
		code |= 2;
		diff -= tempstep;
	}
	tempstep >>= 1;
	if (diff >= tempstep) code |= 1;

	predsample = YM2610_ADPCM_A_Decode(code);

	index += step_adj[code];
	if (index < 0) index = 0;
	if (index > 48) index = 48;

	prevsample = predsample;
	previndex = index;

	return code;
}

//our main sub, init buffers and runs the encode process
//enter this with your sound file loaded into buffer
int Ym2610AEncoder::encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize)
{
	int i;

	//reset to initial conditions
	acc = 0;
	decstep = 0;
	prevsample = 0;
	previndex = 0;
	//watch out for odd data count & allocate buffers
	if (iSampleSize & 1) { iSampleSize++; }
	inBuffer = new short[iSampleSize];
	inBuffer[iSampleSize - 1] = 0x00;

	//fix byte order and downscale data to 12 bits
	for (i = 0; i < iSampleSize; i ++)
	{
		inBuffer[i] = pSrc[i] >> 4;
	}

	//actual encoding
	for (i = 0; i < iSampleSize; i += 2)
	{
		pDis[i / 2] = (byte)((YM2610_ADPCM_A_Encode(inBuffer[i]) << 4) | YM2610_ADPCM_A_Encode(inBuffer[i + 1]));
	}
	delete[] inBuffer;
	return 0;
}
