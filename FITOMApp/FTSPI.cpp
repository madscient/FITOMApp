#include "stdafx.h"
#include "FTSPI.h"

CFTSPI::CFTSPI() : h_libMPSSE(0), m_numChannels(0), p_SPI_GetNumChannels(0), p_SPI_GetChannelInfo(0)
, p_SPI_OpenChannel(0), p_SPI_InitChannel(0), p_SPI_CloseChannel(0), p_SPI_Read(0), p_SPI_Write(0), p_SPI_IsBusy(0), p_SPI_ReadWrite(0)
{
}

CFTSPI::~CFTSPI()
{
	if (h_libMPSSE) {
		FreeModule(h_libMPSSE);
	}
}

BOOL CFTSPI::Init()
{
	h_libMPSSE = LoadLibrary(_T("libMPSSE.dll"));
	if (h_libMPSSE == NULL) {
		return FALSE;
	}

	/* init function pointers */
	p_SPI_GetNumChannels = (pfunc_SPI_GetNumChannels)GetProcAddress(h_libMPSSE, "SPI_GetNumChannels");
	assert(p_SPI_GetNumChannels);
	p_SPI_GetChannelInfo = (pfunc_SPI_GetChannelInfo)GetProcAddress(h_libMPSSE, "SPI_GetChannelInfo");
	assert(p_SPI_GetChannelInfo);
	p_SPI_OpenChannel = (pfunc_SPI_OpenChannel)GetProcAddress(h_libMPSSE, "SPI_OpenChannel");
	assert(p_SPI_OpenChannel);
	p_SPI_InitChannel = (pfunc_SPI_InitChannel)GetProcAddress(h_libMPSSE, "SPI_InitChannel");
	assert(p_SPI_InitChannel);
	p_SPI_Read = (pfunc_SPI_Read)GetProcAddress(h_libMPSSE, "SPI_Read");
	assert(p_SPI_Read);
	p_SPI_Write = (pfunc_SPI_Write)GetProcAddress(h_libMPSSE, "SPI_Write");
	assert(p_SPI_Write);
	p_SPI_CloseChannel = (pfunc_SPI_CloseChannel)GetProcAddress(h_libMPSSE, "SPI_CloseChannel");
	assert(p_SPI_CloseChannel);
	p_SPI_IsBusy = (pfunc_SPI_IsBusy)GetProcAddress(h_libMPSSE, "SPI_IsBusy");
	assert(p_SPI_IsBusy);
	p_SPI_ReadWrite = (pfunc_SPI_ReadWrite)GetProcAddress(h_libMPSSE, "SPI_ReadWrite");
	assert(p_SPI_ReadWrite);

	FT_STATUS status = FT_OK;
	ChannelConfig channelConf = { 0 };

	channelConf.ClockRate = 10000000;	//10MHz
	channelConf.LatencyTimer = 1;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

	uint32 numChannels = 0;
	status = p_SPI_GetNumChannels(&numChannels);
	assert(status == FT_OK);
	//printf("Number of available SPI channels = %d\n", (int)channels);
	if (numChannels) {
		for (uint32 i = 0; i < numChannels; i++) {
			FT_DEVICE_LIST_INFO_NODE devList = { 0 };
			status = p_SPI_GetChannelInfo(i, &devList);
			if (lstrcmp(_T("FTSPI"), devList.Description) == 0) {

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


