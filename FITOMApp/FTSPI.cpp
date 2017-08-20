#include "stdafx.h"
#include "FTSPI.h"

CFTSPI::CFTSPI() : bValid(FALSE), h_libMPSSE(0), m_SPIChannels(0), SPIChannel(NULL), p_SPI_GetNumChannels(0), p_SPI_GetChannelInfo(0)
, p_SPI_OpenChannel(0), p_SPI_InitChannel(0), p_SPI_CloseChannel(0), p_SPI_Read(0), p_SPI_Write(0), p_SPI_IsBusy(0), p_SPI_ReadWrite(0)
{
	bValid = Init();
}

CFTSPI::~CFTSPI()
{
	if (h_libMPSSE) {
		FreeModule(h_libMPSSE);
	}
	if (SPIChannel) {
		delete[] SPIChannel;
	}
}

BOOL CFTSPI::Init()
{
	h_libMPSSE = LoadLibrary(_T("libMPSSE.dll"));
	if (h_libMPSSE == NULL) {
		return FALSE;
	}

	/* init function pointers */
	p_SPI_GetNumChannels = (pfunc_SPI_GetNumChannels)GetProcAddress(h_libMPSSE, _T("SPI_GetNumChannels"));
	assert(p_SPI_GetNumChannels);
	p_SPI_GetChannelInfo = (pfunc_SPI_GetChannelInfo)GetProcAddress(h_libMPSSE, _T("SPI_GetChannelInfo"));
	assert(p_SPI_GetChannelInfo);
	p_SPI_OpenChannel = (pfunc_SPI_OpenChannel)GetProcAddress(h_libMPSSE, _T("SPI_OpenChannel"));
	assert(p_SPI_OpenChannel);
	p_SPI_InitChannel = (pfunc_SPI_InitChannel)GetProcAddress(h_libMPSSE, _T("SPI_InitChannel"));
	assert(p_SPI_InitChannel);
	p_SPI_Read = (pfunc_SPI_Read)GetProcAddress(h_libMPSSE, _T("SPI_Read"));
	assert(p_SPI_Read);
	p_SPI_Write = (pfunc_SPI_Write)GetProcAddress(h_libMPSSE, _T("SPI_Write"));
	assert(p_SPI_Write);
	p_SPI_CloseChannel = (pfunc_SPI_CloseChannel)GetProcAddress(h_libMPSSE, _T("SPI_CloseChannel"));
	assert(p_SPI_CloseChannel);
	p_SPI_IsBusy = (pfunc_SPI_IsBusy)GetProcAddress(h_libMPSSE, _T("SPI_IsBusy"));
	assert(p_SPI_IsBusy);
	p_SPI_ReadWrite = (pfunc_SPI_ReadWrite)GetProcAddress(h_libMPSSE, _T("SPI_ReadWrite"));
	assert(p_SPI_ReadWrite);
	p_FT_WriteGPIO = (pFunc_FT_WriteGPIO)GetProcAddress(h_libMPSSE, _T("FT_WriteGPIO"));

	FT_STATUS status = FT_OK;

	uint32 numChannels = 0;
	status = p_SPI_GetNumChannels(&numChannels);
	assert(status == FT_OK);
	//printf("Number of available SPI channels = %d\n", (int)channels);
	if (numChannels) {
		SPIChannel = new SPIINFO[numChannels];
		for (UINT32 i = 0; i < numChannels; i++) {
			FT_DEVICE_LIST_INFO_NODE devList = { 0 };
			status = p_SPI_GetChannelInfo(i, &devList);
			if (status == FT_OK && lstrcmp(_T("FTSPI"), devList.Description) == 0) {
				FT_HANDLE ftHandle;
				status = SPI_OpenChannel(i, &ftHandle);
				if (status == FT_OK) {
					SPIChannel[m_SPIChannels].index = i;
					SPIChannel[m_SPIChannels].ftHandle = ftHandle;
					m_SPIChannels++;
				}
			}
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

FT_STATUS CFTSPI::SPI_OpenChannel(UINT32 index, FT_HANDLE* handle)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_SPI_OpenChannel) {
		ret = p_SPI_OpenChannel(index, handle);
	}
	return ret;
}

FT_STATUS CFTSPI::SPI_InitChannel(FT_HANDLE handle, ChannelConfig* config)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_SPI_InitChannel) {
		ret = p_SPI_InitChannel(handle, config);
	}
	return ret;
}

FT_STATUS CFTSPI::SPI_CloseChannel(FT_HANDLE handle)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_SPI_CloseChannel) {
		ret = p_SPI_CloseChannel(handle);
	}
	return ret;
}

FT_STATUS CFTSPI::SPI_Read(FT_HANDLE handle, UINT8* buffer, UINT32 sizeToTransfer, UINT32* sizeTransfered, UINT32 options)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_SPI_Read) {
		ret = p_SPI_Read(handle, buffer, sizeToTransfer, (uint32*)sizeTransfered, options);
	}
	return ret;
}

FT_STATUS CFTSPI::SPI_Write(FT_HANDLE handle, UINT8* buffer, UINT32 sizeToTransfer, UINT32* sizeTransfered, UINT32 options)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_SPI_Write) {
		ret = p_SPI_Write(handle, buffer, sizeToTransfer, (uint32*)sizeTransfered, options);
	}
	return ret;
}

UINT32 CFTSPI::GetChannelIndex(UINT32 index)
{
	if (index < m_SPIChannels && SPIChannel != NULL) {
		return SPIChannel[index].index;
	}
	return UINT32(-1);
}

FT_HANDLE CFTSPI::GetChannelHandle(UINT32 index)
{
	if (index < m_SPIChannels && SPIChannel != NULL) {
		return SPIChannel[index].ftHandle;
	}
	return FT_HANDLE(0);
}

FT_STATUS CFTSPI::FT_WriteGPIO(FT_HANDLE handle, uint8 dir, uint8 value)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (p_FT_WriteGPIO) {
		ret = p_FT_WriteGPIO(handle, dir, value);
	}
	return ret;
}
