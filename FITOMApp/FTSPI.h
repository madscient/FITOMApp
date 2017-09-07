#pragma once
#include "../FTDI/ftd2xx.h"

#ifdef LIBMPSSE

#include "../FTDI/libMPSSE_spi.h"

struct SPIINFO {
	UINT32 index;
	FT_HANDLE ftHandle;
};

class CFTSPI {
protected:
	HMODULE h_libMPSSE;
	uint32 m_SPIChannels;
	BOOL bValid;
	SPIINFO* SPIChannel;

	typedef FT_STATUS(*pfunc_SPI_GetNumChannels)(uint32 *numChannels);
	pfunc_SPI_GetNumChannels p_SPI_GetNumChannels;
	typedef FT_STATUS(*pfunc_SPI_GetChannelInfo)(uint32 index, FT_DEVICE_LIST_INFO_NODE *chanInfo);
	pfunc_SPI_GetChannelInfo p_SPI_GetChannelInfo;
	typedef FT_STATUS(*pfunc_SPI_OpenChannel)(uint32 index, FT_HANDLE *handle);
	pfunc_SPI_OpenChannel p_SPI_OpenChannel;
	typedef FT_STATUS(*pfunc_SPI_InitChannel)(FT_HANDLE handle, ChannelConfig *config);
	pfunc_SPI_InitChannel p_SPI_InitChannel;
	typedef FT_STATUS(*pfunc_SPI_CloseChannel)(FT_HANDLE handle);
	pfunc_SPI_CloseChannel p_SPI_CloseChannel;
	typedef FT_STATUS(*pfunc_SPI_Read)(FT_HANDLE handle, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
	pfunc_SPI_Read p_SPI_Read;
	typedef FT_STATUS(*pfunc_SPI_Write)(FT_HANDLE handle, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
	pfunc_SPI_Write p_SPI_Write;
	typedef FT_STATUS(*pfunc_SPI_IsBusy)(FT_HANDLE handle, bool *state);
	pfunc_SPI_IsBusy p_SPI_IsBusy;
	typedef FT_STATUS(*pfunc_SPI_ReadWrite)(FT_HANDLE handle, uint8 *inBuffer, uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred, uint32 transferOptions);
	pfunc_SPI_ReadWrite p_SPI_ReadWrite;
	typedef FT_STATUS(*pFunc_FT_WriteGPIO)(FT_HANDLE handle, uint8 dir, uint8 value);
	pFunc_FT_WriteGPIO p_FT_WriteGPIO;
public:
	CFTSPI();
	~CFTSPI();
	BOOL Init();
	BOOL IsValid() { return bValid; };
	UINT32 GetChannels() { return m_SPIChannels; };
	UINT32 GetChannelIndex(UINT32 index);
	FT_HANDLE GetChannelHandle(UINT32 index);
	FT_STATUS SPI_OpenChannel(UINT32 index, FT_HANDLE* handle);
	FT_STATUS SPI_InitChannel(FT_HANDLE handle, ChannelConfig* config);
	FT_STATUS SPI_CloseChannel(FT_HANDLE handle);
	FT_STATUS SPI_Read(FT_HANDLE handle, UINT8* buffer, UINT32 sizeToTransfer, UINT32* sizeTransfered, UINT32 options);
	FT_STATUS SPI_Write(FT_HANDLE handle, UINT8* buffer, UINT32 sizeToTransfer, UINT32* sizeTransfered, UINT32 options);
	FT_STATUS FT_WriteGPIO(FT_HANDLE handle, uint8 dir, uint8 value);
};
#else
class SPIINFO {
public:
	SPIINFO() : index(0), ftHandle(0), wptr(0), rptr(0) {};
	UINT32 index;
	FT_HANDLE ftHandle;
	BYTE cmdbuf[65536];
	int wptr;
	int rptr;
};

class CFTSPI {
protected:
	BOOL bValid;
	std::vector<SPIINFO&> SPIChannel;
	void SPI_Push(UINT32 index, BYTE data);
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
};

#endif

