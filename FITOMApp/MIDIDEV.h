#ifndef __MIDIDEV_H__
#define __MIDIDEV_H__

#include "Port.h"

class CMidiInst;

class CMidiIn {
protected:
	CMidiInst* pMidiInst;
public:
	CMidiIn() : pMidiInst(0) {};
	~CMidiIn() {};
	virtual BOOL RegistInterrupt(CMidiInst* pInst) { return FALSE; }
	virtual uint8_t IsReceived() = 0;
	virtual uint8_t Read() = 0;
	virtual const char* GetDescriptor() = 0;
};

class CMidiOut {
	uint8_t buf[4096];
	int	len;
public:
	CMidiOut() : len(0) {};
	~CMidiOut() {};
	virtual uint8_t IsTxBusy() = 0;
	virtual void Send(uint8_t data) = 0;
	virtual void Send(void* msg, size_t len) = 0;
	virtual const char* GetDescriptor() = 0;
};

#ifdef _PC9801
class CMPU401 : public CMidiIn, public CMidiOut
{
protected:
	char desc[32];
public:
	CMPU401(uint16_t dtport, uint16_t stport);
	~CMPU401();
	virtual uint8_t	IsReceived();
	virtual uint8_t	IsTxBusy();
	virtual uint8_t	Read();
	virtual void Send(uint8_t data);
	virtual void Send(void* msg, size_t len);
	uint8_t	GetStat();
	void	SetStat(uint8_t data);
	virtual const char* GetDescriptor() { return desc; };
protected:
	uint16_t portd;
	uint16_t ports;
};

class CYMZ263Midi : public CMidiIn, public CMidiOut
{
protected:
	char desc[32];
public:
	CYMZ263Midi(CPort* devport);
	~CYMZ263Midi();
	virtual uint8_t	IsReceived();
	virtual uint8_t	Read();
	virtual uint8_t	IsTxBusy();
	virtual void Send(uint8_t data);
	virtual void Send(void* msg, size_t len);
	virtual const char* GetDescriptor() { return desc; };
protected:
	CPort* port;
};

class CRSMidi : public CMidiIn, public CMidiOut
{
protected:
	char desc[32];
public:
	CRSMidi();
	~CRSMidi();
	virtual uint8_t	IsReceived();
	virtual uint8_t	IsTxBusy();
	virtual uint8_t	Read();
	virtual void Send(uint8_t data);
	virtual void Send(void* msg, size_t len);
	virtual const char* GetDescriptor() { return desc; };
};

#endif

#ifdef _WIN32

#define RING_MAX	8192

class CW32MidiIn : public CMidiIn
{
public:
	CW32MidiIn(LPCTSTR name);
	~CW32MidiIn();
	virtual uint8_t	IsReceived();
	virtual uint8_t	Read();
	static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	virtual const char* GetDescriptor() { return desc; };
	virtual BOOL RegistInterrupt(CMidiInst* pInst) { pMidiInst = pInst; return TRUE; };
protected:
	int AddBuffer(BYTE msg);
	int AddBuffer(DWORD msg);
	int AddBuffer(MIDIHDR msg);
	int InstProc(DWORD msg);
	int InstProc(MIDIHDR msg);
	HMIDIIN hIn;
	MIDIHDR mhdr;
	uint8_t buf[RING_MAX];
	DWORD rpt;
	DWORD wpt;
	char desc[32];
};

class CW32MidiOut : public CMidiOut
{
public:
	CW32MidiOut(LPCTSTR name);
	~CW32MidiOut();
	virtual uint8_t IsTxBusy();
	virtual void Send(uint8_t data);
	virtual void Send(void* msg, size_t len);
	virtual const char* GetDescriptor() { return desc; };
	static void CALLBACK MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
protected:
	HMIDIOUT hOut;
	uint32_t txout;
	char desc[32];
};

class CW32RsMidi : public CMidiIn, public CMidiOut
{
protected:
	HANDLE hCom;
	DCB dcb;
	char desc[32];
public:
	CW32RsMidi(LPCTSTR name);
	~CW32RsMidi();
	virtual uint8_t	IsReceived();
	virtual uint8_t	Read();
	virtual uint8_t	IsTxBusy();
	virtual void Send(uint8_t data);
	virtual void Send(void* msg, size_t len);
	virtual const char* GetDescriptor();
};
#endif

#ifdef _LINUX
class CUartMidiIn : public CMidiIn
{
protected:
	char desc[32];
	int hUart;
public:
	CUartMidiIn(const char* param);
	~CUartMidiIn();
	virtual uint8_t	IsReceived();
	virtual uint8_t	Read();
	virtual const char* GetDescriptor() { return desc; };
};
#endif

#endif
