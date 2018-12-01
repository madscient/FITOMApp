#include "stdafx.h"
#include "FTSPI.h"

CFTSPI::CFTSPI()
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
		if (devList[i].Type == FT_DEVICE_232H) {
			sprintf_s(description, _countof(description), _T("%s(%s)"), devList[i].Description, devList[i].SerialNumber);
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
		//ret = FT_SetBaudRate(SPIChannel[index].ftHandle, 2000000L);	//1Mbps
		//if (ret != FT_OK) return ret;
		ret = FT_Purge(SPIChannel[index].ftHandle, FT_PURGE_RX | FT_PURGE_TX);
		if (ret != FT_OK) return ret;
		::Sleep(10);
		ret = FT_SetLatencyTimer(SPIChannel[index].ftHandle, 1);	//1ms
		if (ret != FT_OK) return ret;
		ret = FT_SetBitMode(SPIChannel[index].ftHandle, 0x0, 0x00);	//reset mode
		if (ret != FT_OK) return ret;
		::Sleep(10);
		ret = FT_SetBitMode(SPIChannel[index].ftHandle, 0x00, 0x02);	//MPSSE mode
		if (ret != FT_OK) return ret;
		ret = FT_Purge(SPIChannel[index].ftHandle, FT_PURGE_RX);
		if (ret != FT_OK) return ret;
		::Sleep(20);
		SPI_Push(index, 0x80);	//Set initial value of ADBUS
		SPI_Push(index, 0xf8);	//
		SPI_Push(index, 0xfb);	//
		SPI_Push(index, 0x82);	//Set initial value of ACBUS
		SPI_Push(index, 0xff);	//
		SPI_Push(index, 0xff);	//
		SPI_Push(index, 0x8a);	//disable initial devide
		SPI_Push(index, 0x86);	//CK devisor
		SPI_Push(index, 0x02);
		SPI_Push(index, 0x00);
		SPI_Push(index, 0x85);	//disable loopback
		SPI_Push(index, 0x8d);	//disable 3 phase clocking
		ret = SPI_Flush(index);
		::Sleep(100);
		if (ret != FT_OK) return ret;
	}
	return ret;
}

void CFTSPI::SPI_Push(UINT32 index, BYTE data)
{
	SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = data;
	if (SPIChannel[index].wptr == (BUFSIZE-1)) { SPI_Flush(index); }
}

void CFTSPI::SPI_Push(UINT32 index, BYTE* data, UINT32 length)
{
	if ((SPIChannel[index].wptr + length) >= (BUFSIZE-1)) { SPI_Flush(index); }
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
		UINT16 length = sizeToTransfer-1;
		SPI_Push(index, 0x80);	//assert CS
		SPI_Push(index, 0xf8 & csmask);	//
		SPI_Push(index, 0xfb);	//
		SPI_Push(index, 0x11);	//ve+ edge MSB first no read
		SPI_Push(index, (BYTE*)(&length), 2);	//length
		SPI_Push(index, buffer, sizeToTransfer);
		SPI_Push(index, 0x80);	//dessert CS
		SPI_Push(index, 0xf8);	//
		SPI_Push(index, 0xfb);	//
		SPI_Flush(index);
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
		SPI_Flush(index);
	}
	return ret;
}

FT_STATUS CFTSPI::SPI_Flush(UINT32 index)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (index < SPIChannel.size()) {
		DWORD written = 0;
		if (SPIChannel[index].wptr) {
			//SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = 0x87;	//force flush
			ret = FT_Write(SPIChannel[index].ftHandle, SPIChannel[index].cmdbuf, SPIChannel[index].wptr, &written);
			SPIChannel[index].wptr = 0;
			//BYTE forceflush = 0x87;
			//ret = FT_Write(SPIChannel[index].ftHandle, &forceflush, 1, &written);
		}
		else {
			ret = FT_OK;
		}
	}
	return ret;
}

void CFTSPI::SPI_Flush()
{
	for (UINT32 i = 0; i < SPIChannel.size(); i++) {
		FT_STATUS status = SPI_Flush(i);
		assert(status == FT_OK);
	}
}

void CFTSPI::InitialClear()
{
	for (int index = 0; index < SPIChannel.size(); index++) {
		SPI_Push(index, 0x82);	//Set high byte
		SPI_Push(index, 0x00);	//assert IC
		SPI_Push(index, 0xff);	//dir
		SPI_Flush(index);
		::Sleep(10);
		SPI_Push(index, 0x82);	//Set high byte
		SPI_Push(index, 0xff);	//dessert IC
		SPI_Push(index, 0xff);	//dir
		SPI_Flush(index);
	}
}

void CFTSPI::GetInterfaceDesc(TCHAR* str, int len)
{
	sprintf_s(str, len, _T("%s"), description);
}