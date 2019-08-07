#include "stdafx.h"
#include "MIDIDEV.h"
#include <MMSystem.h>
#include "SoundDev.h"
#include "MIDI.h"

CW32MidiIn::CW32MidiIn(LPCTSTR name) : rpt(0), wpt(0)
{
	MIDIINCAPS mic;
	int dev = -1;
	int numdevs = midiInGetNumDevs();
	for (int i=0; i<numdevs; i++) {
		midiInGetDevCaps(i, &mic, sizeof(mic));
		if (strcmp(name, mic.szPname) == 0) {
			dev = i;
			break;
		}
	}
	StringCchCopy(desc, 32, mic.szPname);
	MMRESULT res = midiInOpen(&hIn, dev, (DWORD_PTR)CW32MidiIn::MidiInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (res != MMSYSERR_NOERROR) { throw new CResourceException() ; }
	mhdr.dwFlags = 0;
	mhdr.lpData = (LPSTR)(new uint8_t[RING_MAX]);
	mhdr.dwBufferLength = RING_MAX;
	midiInPrepareHeader(hIn, &mhdr, sizeof(MIDIHDR));
	midiInStart(hIn);
}

CW32MidiIn::~CW32MidiIn()
{
	midiInStop(hIn);
	midiInReset(hIn);
	midiInUnprepareHeader(hIn, &mhdr, sizeof(MIDIHDR));
	midiInClose(hIn);
}

uint8_t CW32MidiIn::IsReceived(void)
{
    return (wpt!=rpt);
}

uint8_t CW32MidiIn::Read(void)
{
	uint8_t ret = 0;
	ret = buf[rpt];
	rpt = (rpt + 1) & (RING_MAX - 1);
	return ret;
}

int CW32MidiIn::AddBuffer(BYTE msg)
{
	buf[wpt] = msg;
	wpt = (wpt + 1) & (RING_MAX - 1);
	return 0;
}

int CW32MidiIn::AddBuffer(DWORD msg)
{
	buf[wpt] = LOBYTE(LOWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	buf[wpt] = HIBYTE(LOWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	buf[wpt] = LOBYTE(HIWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	return 0;
}

int CW32MidiIn::AddBuffer(MIDIHDR msg)
{
	for (int i = 0; i < msg.dwBytesRecorded; i++) {
		AddBuffer((BYTE)msg.lpData[i]);
	}
	return 0;
}

int CW32MidiIn::InstProc(DWORD msg)
{
	buf[0] = LOBYTE(LOWORD(msg));
	buf[1] = HIBYTE(LOWORD(msg));
	buf[2] = LOBYTE(HIWORD(msg));
	pMidiInst->InterruptCallBack(buf, 3);
	return 0;
}

int CW32MidiIn::InstProc(MIDIHDR msg)
{
	pMidiInst->InterruptCallBack(LPBYTE(msg.lpData), msg.dwBytesRecorded);
	return 0;
}

void CALLBACK CW32MidiIn::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch(wMsg) {
	case MIM_DATA:
		{
			CW32MidiIn* mpu = (CW32MidiIn*)dwInstance;
			if (mpu->pMidiInst) {
				mpu->InstProc(dwParam1);
			}
			else {
				mpu->AddBuffer(dwParam1);
			}
		}
		break;
	case MIM_OPEN:		// 入力デバイス オープン コールバック
		break;
	case MIM_CLOSE:		// 入力デバイスをクローズ
		break;
	case MIM_LONGDATA:	// 入力バッファのコールバック
		{
			CW32MidiIn* mpu = (CW32MidiIn*)dwInstance;
			if (0 /*mpu->pMidiInst*/) {
				mpu->InstProc(*(LPMIDIHDR(dwParam1)));
			}
			else {
				mpu->AddBuffer(*(LPMIDIHDR(dwParam1)));
			}
		}
		break;
	case MIM_ERROR:		// 入力デバイス エラー コールバック
		break;
	case MIM_LONGERROR:	// 無効なシステム エクスクルーシブ メッセージに対するコールバック
		break;
	case MIM_MOREDATA:	// ???
		break;
	default:
		break;
	}
}

CW32MidiOut::CW32MidiOut(LPCTSTR name) : txout(0)
{
	MIDIOUTCAPS moc;
	int dev = -1;
	int numdevs = midiInGetNumDevs();
	for (int i=0; i<numdevs; i++) {
		midiOutGetDevCaps(i, &moc, sizeof(moc));
		if (strcmp(name, moc.szPname) == 0) {
			dev = i;
			break;
		}
	}
	if (dev < 0) {
		dev = 0;
	}
	StringCchCopy(desc, 32, moc.szPname);
	midiOutOpen(&hOut, dev, (DWORD_PTR)CW32MidiOut::MidiOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
}

void CALLBACK CW32MidiOut::MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (wMsg) {
	case MOM_DONE:
		{
			CW32MidiOut* mpu = (CW32MidiOut*)dwInstance;
			LPMIDIHDR mhdr = (LPMIDIHDR)dwParam1;
			midiOutUnprepareHeader(hMidiOut, mhdr, sizeof(MIDIHDR));
			delete [] mhdr->lpData;
			delete mhdr;
			mpu->txout--;
		}
		break;
	}
}

void CW32MidiOut::Send(uint8_t data)
{
	Send(&data, 1);
}

void CW32MidiOut::Send(void* msg, size_t len)
{
	if (len <= 4) {
		midiOutShortMsg(hOut, *((DWORD*)msg));
	} else {
		txout++;
		LPMIDIHDR mhdr = new MIDIHDR;
		mhdr->dwFlags = 0;
		mhdr->lpData = (LPSTR)(new uint8_t[len]);
		mhdr->dwBufferLength = len;
		memcpy(mhdr->lpData, msg, len);
		midiOutPrepareHeader(hOut, mhdr, sizeof(MIDIHDR));
		midiOutLongMsg(hOut, mhdr, sizeof(MIDIHDR));
	}
}

uint8_t CW32MidiOut::IsTxBusy()
{
	return uint8_t(txout);
}

CW32RsMidi::CW32RsMidi(LPCTSTR name)
{
	hCom = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); // シリアルポートを開く
	GetCommState(hCom, &dcb); // 現在の設定値を読み込み

	dcb.BaudRate = 31500; // 速度
	dcb.ByteSize = 8; // データ長
	dcb.Parity = NOPARITY; // パリティ
	dcb.StopBits = ONESTOPBIT; // ストップビット長
	dcb.fOutxCtsFlow = FALSE; // 送信時CTSフロー
	dcb.fRtsControl = FALSE; // RTSフロー

	SetCommState(hCom, &dcb); // 変更した設定値を書き込み
	StringCchPrintf(desc, 32, _T("%1 RS-MIDI(31500bps)"), name);
}

const char* CW32RsMidi::GetDescriptor()
{
	return desc;
}

CW32RsMidi::~CW32RsMidi()
{
	CloseHandle(hCom);
}

uint8_t CW32RsMidi::IsReceived()
{
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(hCom, &errors, &comStat);
	return (comStat.cbInQue > 0);
}

uint8_t CW32RsMidi::Read()
{
	uint8_t ret = 0;
	DWORD numberOfPut;
	ReadFile(hCom, &ret, sizeof(ret), &numberOfPut, NULL);
	return ret;
}

uint8_t CW32RsMidi::IsTxBusy()
{
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(hCom, &errors, &comStat);
	return (comStat.cbOutQue > 0);
}

void CW32RsMidi::Send(void* msg, size_t len)
{
	DWORD ret = 0;
	WriteFile(hCom, msg, len, &ret, NULL);
}

void CW32RsMidi::Send(uint8_t data)
{
	Send(&data, 1);
}
