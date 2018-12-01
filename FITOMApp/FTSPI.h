#pragma once
#include <vector>
#include "../FTDI/ftd2xx.h"

#define BUFSIZE 65536
class SPIINFO {
public:
	SPIINFO() : index(0), ftHandle(0), wptr(0), rptr(0) {};
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
public:
	virtual BOOL IsValid() { return bValid; };
	virtual UINT32 GetChannels() = 0;
	virtual UINT32 GetChannelIndex(UINT32 index) = 0;
	virtual void InitialClear() = 0;
	virtual void GetInterfaceDesc(TCHAR* str, int len) = 0;
	virtual FT_HANDLE GetChannelHandle(UINT32 index) = 0;
};

class CFTSPI : public CFTInterface {
protected:
	std::vector<SPIINFO> SPIChannel;
	void SPI_Push(UINT32 index, BYTE data);
	void SPI_Push(UINT32 index, BYTE* buf, UINT32 length);
public:
	CFTSPI();
	~CFTSPI();
	virtual BOOL Init();
	virtual UINT32 GetChannels() { return SPIChannel.size(); };
	virtual UINT32 GetChannelIndex(UINT32 index);
	virtual void InitialClear();
	virtual void GetInterfaceDesc(TCHAR* str, int len);
	virtual FT_HANDLE GetChannelHandle(UINT32 index);
	FT_STATUS SPI_OpenChannel(UINT32 index, FT_HANDLE* handle);
	FT_STATUS SPI_InitChannel(UINT32 index);
	FT_STATUS SPI_CloseChannel(UINT32 index);
	FT_STATUS SPI_Read(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	FT_STATUS SPI_Write(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	FT_STATUS FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value);
	FT_STATUS SPI_Flush(UINT32 index);
	void SPI_Flush();
};


