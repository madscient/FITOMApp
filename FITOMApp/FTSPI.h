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

class CFTSPI {
protected:
	BOOL bValid;
	std::vector<SPIINFO> SPIChannel;
	void SPI_Push(UINT32 index, BYTE data);
	void SPI_Push(UINT32 index, BYTE* buf, UINT32 length);
public:
	CFTSPI();
	~CFTSPI();
	BOOL Init();
	BOOL IsValid() { return bValid; };
	UINT32 GetChannels() { return SPIChannel.size(); };
	UINT32 GetChannelIndex(UINT32 index);
	FT_HANDLE GetChannelHandle(UINT32 index);
	FT_STATUS SPI_OpenChannel(UINT32 index, FT_HANDLE* handle);
	FT_STATUS SPI_InitChannel(UINT32 index);
	FT_STATUS SPI_CloseChannel(UINT32 index);
	FT_STATUS SPI_Read(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	FT_STATUS SPI_Write(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs);
	FT_STATUS FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value);
	FT_STATUS Flush(UINT32 index);
	void Flush();
};


