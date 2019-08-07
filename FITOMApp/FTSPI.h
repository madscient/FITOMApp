#pragma once
#include <vector>
#include "../FTDI/ftd2xx.h"

#define BUFSIZE 65536

int InitFTInterface();

class FTHINFO {
public:
	FTHINFO() : index(0), ftHandle(0), wptr(0), rptr(0) {};
	FTHINFO& operator=(FTHINFO& fth);
	uint32_t index;
	FT_HANDLE ftHandle;
	TCHAR description[96];
	TCHAR serialnumber[96];
	BYTE cmdbuf[BUFSIZE];
	int wptr;
	int rptr;
};

class CFTInterface {
protected:
	BOOL bValid;
	FTHINFO FTChannel;
	virtual void BufferPush(BYTE data);
	virtual void BufferPush(BYTE* buf, uint32_t length);
public:
	CFTInterface();
	~CFTInterface();
	virtual BOOL IsValid() { return bValid; };
	virtual void GetInterfaceDesc(TCHAR* str, int len);
	virtual FT_HANDLE GetHandle() { return FTChannel.ftHandle; };
	virtual FT_STATUS BufferFlush();
	//to be implemented on derived class
	virtual FT_STATUS Init() { return FT_NOT_SUPPORTED; };
	virtual void InitialClear() = 0;
	virtual FT_STATUS BufferedRead(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs) = 0;
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value) = 0;
};

class CFTSPI : public CFTInterface {
public:
	CFTSPI(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
};

class CFTHBE : public CFTInterface {
public:
	CFTHBE(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
};
