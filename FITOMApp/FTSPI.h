#pragma once
#include <vector>
#include "../FTDI/ftd2xx.h"

#define BUFSIZE 65536
class FTHINFO {
public:
	FTHINFO() : index(0), ftHandle(0), wptr(0), rptr(0) {};
	UINT32 index;
	FT_HANDLE ftHandle;
	BYTE cmdbuf[BUFSIZE];
	int wptr;
	int rptr;
};

class CFTInterface {
protected:
	BOOL bValid;
	TCHAR description[96];
	std::vector<FTHINFO> FTChannel;
	virtual void BufferPush(UINT32 index, BYTE data);
	virtual void BufferPush(UINT32 index, BYTE* buf, UINT32 length);
public:
	CFTInterface();
	~CFTInterface();
	virtual BOOL IsValid() { return bValid; };
	virtual UINT32 GetChannels() { return FTChannel.size(); };
	virtual UINT32 GetChannelIndex(UINT32 index);
	virtual void GetInterfaceDesc(TCHAR* str, int len);
	virtual FT_HANDLE GetChannelHandle(UINT32 index);
	virtual void BufferFlush();
	//to be implemented on derived class
	virtual BOOL Init() { return false; };
	virtual void InitialClear() = 0;
	//virtual FT_STATUS OpenChannel(UINT32 index, FT_HANDLE* handle) = 0;
	virtual FT_STATUS InitChannel(UINT32 index) = 0;
	//virtual FT_STATUS CloseChannel(UINT32 index) = 0;
	virtual FT_STATUS BufferedRead(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	virtual FT_STATUS BufferedWrite(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs) = 0;
	virtual FT_STATUS FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value) = 0;
	virtual FT_STATUS BufferFlush(UINT32 index) = 0;
};

class CFTSPI : public CFTInterface {
public:
	CFTSPI() : CFTInterface() {};
	virtual BOOL Init();
	virtual void InitialClear();
	//virtual FT_STATUS OpenChannel(UINT32 index, FT_HANDLE* handle);
	virtual FT_STATUS InitChannel(UINT32 index);
	//virtual FT_STATUS CloseChannel(UINT32 index);
	virtual FT_STATUS BufferedWrite(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	virtual FT_STATUS FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value);
	virtual FT_STATUS BufferFlush(UINT32 index);
};


