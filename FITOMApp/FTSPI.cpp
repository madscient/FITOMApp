#include "stdafx.h"
#include "FTSPI.h"

CFTInterface::CFTInterface()
{
	bValid = Init();
}

CFTInterface::~CFTInterface()
{
	for (int i = 0; i < FTChannel.size(); i++) {
		FT_STATUS status = FT_Close(FTChannel[i].ftHandle);
	}
}

void CFTInterface::BufferPush(UINT32 index, BYTE data)
{
	FTChannel[index].cmdbuf[FTChannel[index].wptr++] = data;
	if (FTChannel[index].wptr == (BUFSIZE - 1)) { BufferFlush(index); }
}

void CFTInterface::BufferPush(UINT32 index, BYTE* data, UINT32 length)
{
	if ((FTChannel[index].wptr + length) >= (BUFSIZE - 1)) { BufferFlush(index); }
	memcpy(&FTChannel[index].cmdbuf[FTChannel[index].wptr], data, length);
	FTChannel[index].wptr += length;
}

FT_STATUS CFTInterface::BufferedRead(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	return ret;
}

UINT32 CFTInterface::GetChannelIndex(UINT32 index)
{
	if (index < FTChannel.size()) {
		return FTChannel[index].index;
	}
	return UINT32(-1);
}

FT_HANDLE CFTInterface::GetChannelHandle(UINT32 index)
{
	if (index < FTChannel.size() != NULL) {
		return FTChannel[index].ftHandle;
	}
	return FT_HANDLE(0);
}

void CFTInterface::GetInterfaceDesc(TCHAR* str, int len)
{
	sprintf_s(str, len, _T("%s"), description);
}

void CFTInterface::BufferFlush()
{
	for (UINT32 i = 0; i < FTChannel.size(); i++) {
		FT_STATUS status = BufferFlush(i);
		assert(status == FT_OK);
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
			FTHINFO spiinfo;
			spiinfo.index = i;
			spiinfo.rptr = spiinfo.wptr = 0;
			status = FT_Open(i, &spiinfo.ftHandle);
			assert(status == FT_OK);
			FTChannel.push_back(spiinfo);
			status = InitChannel(FTChannel.size() - 1);
			assert(status == FT_OK);
		}
	}
	delete[] devList;
	return (numDevices != 0);
}

FT_STATUS CFTSPI::InitChannel(UINT32 index)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (index < FTChannel.size()) {
		//ret = FT_SetBaudRate(SPIChannel[index].ftHandle, 2000000L);	//1Mbps
		//if (ret != FT_OK) return ret;
		ret = FT_Purge(FTChannel[index].ftHandle, FT_PURGE_RX | FT_PURGE_TX);
		if (ret != FT_OK) return ret;
		::Sleep(10);
		ret = FT_SetLatencyTimer(FTChannel[index].ftHandle, 1);	//1ms
		if (ret != FT_OK) return ret;
		ret = FT_SetBitMode(FTChannel[index].ftHandle, 0x0, 0x00);	//reset mode
		if (ret != FT_OK) return ret;
		::Sleep(10);
		ret = FT_SetBitMode(FTChannel[index].ftHandle, 0x00, 0x02);	//MPSSE mode
		if (ret != FT_OK) return ret;
		ret = FT_Purge(FTChannel[index].ftHandle, FT_PURGE_RX);
		if (ret != FT_OK) return ret;
		::Sleep(20);
		BufferPush(index, 0x80);	//Set initial value of ADBUS
		BufferPush(index, 0xf8);	//
		BufferPush(index, 0xfb);	//
		BufferPush(index, 0x82);	//Set initial value of ACBUS
		BufferPush(index, 0xff);	//
		BufferPush(index, 0xff);	//
		BufferPush(index, 0x8a);	//disable initial devide
		BufferPush(index, 0x86);	//CK devisor
		BufferPush(index, 0x02);
		BufferPush(index, 0x00);
		BufferPush(index, 0x85);	//disable loopback
		BufferPush(index, 0x8d);	//disable 3 phase clocking
		ret = BufferFlush(index);
		::Sleep(100);
		if (ret != FT_OK) return ret;
	}
	return ret;
}


FT_STATUS CFTSPI::BufferedWrite(UINT32 index, UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	UINT8 csmask = ~(1 << (cs + 3));
	if (index < FTChannel.size() && sizeToTransfer) {
		UINT16 length = sizeToTransfer-1;
		BufferPush(index, 0x80);	//assert CS
		BufferPush(index, 0xf8 & csmask);	//
		BufferPush(index, 0xfb);	//
		BufferPush(index, 0x11);	//ve+ edge MSB first no read
		BufferPush(index, (BYTE*)(&length), 2);	//length
		BufferPush(index, buffer, sizeToTransfer);
		BufferPush(index, 0x80);	//dessert CS
		BufferPush(index, 0xf8);	//
		BufferPush(index, 0xfb);	//
		BufferFlush(index);
	}
	return ret;
}

FT_STATUS CFTSPI::FT_WriteGPIO(UINT32 index, UINT8 dir, UINT8 value)
{
	FT_STATUS ret = FT_OK;
	if (index < FTChannel.size()) {
		BufferPush(index, 0x82);	//Set high byte
		BufferPush(index, value);	//
		BufferPush(index, dir);	//
		BufferFlush(index);
	}
	return ret;
}

FT_STATUS CFTSPI::BufferFlush(UINT32 index)
{
	FT_STATUS ret = FT_OTHER_ERROR;
	if (index < FTChannel.size()) {
		DWORD written = 0;
		if (FTChannel[index].wptr) {
			//SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = 0x87;	//force flush
			ret = FT_Write(FTChannel[index].ftHandle, FTChannel[index].cmdbuf, FTChannel[index].wptr, &written);
			FTChannel[index].wptr = 0;
			//BYTE forceflush = 0x87;
			//ret = FT_Write(SPIChannel[index].ftHandle, &forceflush, 1, &written);
		}
		else {
			ret = FT_OK;
		}
	}
	return ret;
}

void CFTSPI::InitialClear()
{
	for (int index = 0; index < FTChannel.size(); index++) {
		BufferPush(index, 0x82);	//Set high byte
		BufferPush(index, 0x00);	//assert IC
		BufferPush(index, 0xff);	//dir
		BufferFlush(index);
		::Sleep(10);
		BufferPush(index, 0x82);	//Set high byte
		BufferPush(index, 0xff);	//dessert IC
		BufferPush(index, 0xff);	//dir
		BufferFlush(index);
	}
}
