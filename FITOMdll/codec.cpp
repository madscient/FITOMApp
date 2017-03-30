#include "stdafx.h"
#include "codec.h"
#include <math.h>

static long stepsizeTable[16] =
{
	57, 57, 57, 57, 77, 102, 128, 153,
	57, 57, 57, 57, 77, 102, 128, 153
};

// コンストラクタ
Adpcm::Adpcm(){
}

// デストラクタ
Adpcm::~Adpcm(){
}

// ADPCM変換
BYTE* Adpcm::waveToAdpcm(void *pData, DWORD dSize, DWORD &dAdpcmSize, DWORD dHiRate){
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
	short	*pPcm = resampling(dPcmSize, dHiRate);
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
short* Adpcm::resampling(DWORD &dSize, DWORD dHiRate){
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
	int iDisRate = 16000;	// 出力のサンプルレート
	if (dHiRate != 0){
		iDisRate = 32000;
	}
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
int Adpcm::encode(short *pSrc, unsigned char *pDis, DWORD iSampleSize){
	static long stepsizeTable[16] =
	{
		57, 57, 57, 57, 77, 102, 128, 153,
		57, 57, 57, 57, 77, 102, 128, 153
	};
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

