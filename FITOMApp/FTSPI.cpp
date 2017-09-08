#include "stdafx.h"
#include "FTSPI.h"

CFTSPI::CFTSPI() : bValid(FALSE)
{
	bValid = Init();
}

CFTSPI::~CFTSPI()
{
	for (int i = 0; i < SPIChannel.size(); i++) {
		FT_STATUS status = FT_Close(SPIChannel[i].ftHandle);
	}
}

BOOL CFTSPI::Init()
{
	FT_STATUS status = FT_OK;

	DWORD numDevices = 0;
	status = FT_ListDevices(&numDevices, 0, FT_LIST_NUMBER_ONLY);
	assert(status == FT_OK);
	//printf("Number of available SPI channels = %d\n", (int)channels);
	status = FT_CreateDeviceInfoList(&numDevices);
	assert(status == FT_OK);
	FT_DEVICE_LIST_INFO_NODE* devList = new FT_DEVICE_LIST_INFO_NODE[numDevices];
	status = FT_GetDeviceInfoList(devList, &numDevices);
	assert(status == FT_OK);

	for (DWORD i = 0; i < numDevices; i++) {
		if (lstrcmp(devList[i].Description, _T("FTSPI")) == 0) {
			SPIINFO spiinfo;
			spiinfo.index = i;
			spiinfo.rptr = spiinfo.wptr = 0;
			status = FT_Open(i, &spiinfo.ftHandle);
			assert(status == FT_OK);
			SPIChannel.push_back(spiinfo);
			status = SPI_InitChannel(SPIChannel.size() - 1);
			assert(status == FT_OK);
		}
	}
	delete[] devList;
	return (numDevices != 0);
}

FT_STATUS CFTSPI::SPI_InitChannel(UINT32 index)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (index < SPIChannel.size()) {
		ret = FT_SetBaudRate(SPIChannel[index].ftHandle, 1000000L);	//1Mbps
		if (ret != FT_OK) return ret;
		ret = FT_Purge(SPIChannel[index].ftHandle, FT_PURGE_RX | FT_PURGE_TX);
		if (ret != FT_OK) return ret;
		ret = FT_SetLatencyTimer(SPIChannel[index].ftHandle, 1);	//1ms
		if (ret != FT_OK) return ret;
		ret = FT_SetBitMode(SPIChannel[index].ftHandle, 0x0, 0x00);	//reset mode
		if (ret != FT_OK) return ret;
		::Sleep(10);
		ret = FT_SetBitMode(SPIChannel[index].ftHandle, 0xfb, 0x02);	//MPSSE mode
		if (ret != FT_OK) return ret;
		ret = FT_Purge(SPIChannel[index].ftHandle, FT_PURGE_RX);
		if (ret != FT_OK) return ret;
		::Sleep(20);
		SPI_Push(index, 0x80);	//Set initial value of ADBUS
		SPI_Push(index, 0xf8);	//
		SPI_Push(index, 0xfb);	//
		SPI_Push(index, 0x8b);	//enable initial devide
		SPI_Push(index, 0x86);	//CK devisor
		SPI_Push(index, 0x05);
		SPI_Push(index, 0x00);
		SPI_Push(index, 0x85);	//disable loopback
		SPI_Push(index, 0x8d);	//disable 3 phase clocking
		ret = Flush(index);
		if (ret != FT_OK) return ret;
	}
	return ret;
}

void CFTSPI::SPI_Push(UINT32 index, BYTE data)
{
	SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = data;
	if (SPIChannel[index].wptr == (BUFSIZE-1)) { Flush(index); }
}

void CFTSPI::SPI_Push(UINT32 index, BYTE* data, UINT32 length)
{
	if ((SPIChannel[index].wptr + length) >= (BUFSIZE-1)) { Flush(index); }
	memcpy(&SPIChannel[index].cmdbuf[SPIChannel[index].wptr], data, length);
	SPIChannel[index].wptr += length;
}

FT_STATUS CFTSPI::SPI_Read(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	return ret;
}

FT_STATUS CFTSPI::SPI_Write(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	UINT8 csmask = ~(1 << (cs + 3));
	if (index < SPIChannel.size() && sizeToTransfer) {
		sizeToTransfer--;
		SPI_Push(index, 0x80);	//assert CS
		SPI_Push(index, 0xf8 & csmask);	//
		SPI_Push(index, 0xfb);	//
		SPI_Push(index, 0x10);	//ve+ edge MSB first no read
		SPI_Push(index, sizeToTransfer & 0xff);	//length LSB
		SPI_Push(index, (sizeToTransfer >> 8) & 0xff);	//length MSB
		SPI_Push(index, buffer, sizeToTransfer+1);
		SPI_Push(index, 0x80);	//dessert CS
		SPI_Push(index, 0xf8);	//
		SPI_Push(index, 0xfb);	//
	}
	return ret;
}

UINT32 CFTSPI::GetChannelIndex(UINT32 index)
{
	if (index < SPIChannel.size()) {
		return SPIChannel[index].index;
	}
	return UINT32(-1);
}

FT_HANDLE CFTSPI::GetChannelHandle(UINT32 index)
{
	if (index < SPIChannel.size() != NULL) {
		return SPIChannel[index].ftHandle;
	}
	return FT_HANDLE(0);
}

FT_STATUS CFTSPI::FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value)
{
	FT_STATUS ret = FT_OK;
	if (index < SPIChannel.size()) {
		SPI_Push(index, 0x82);	//Set high byte
		SPI_Push(index, value);	//
		SPI_Push(index, dir);	//
		Flush(index);
	}
	return ret;
}

FT_STATUS CFTSPI::Flush(UINT32 index)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (index < SPIChannel.size()) {
		DWORD written = 0;
		if (SPIChannel[index].wptr) {
			//SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = 0x87;	//force flush
			ret = FT_Write(SPIChannel[index].ftHandle, SPIChannel[index].cmdbuf, SPIChannel[index].wptr, &written);
			SPIChannel[index].wptr = 0;
		}
		else {
			ret = FT_OK;
		}
	}
	return ret;
}

void CFTSPI::Flush()
{
	for (UINT32 i = 0; i < SPIChannel.size(); i++) {
		FT_STATUS status = Flush(i);
		assert(status == FT_OK);
	}
}
