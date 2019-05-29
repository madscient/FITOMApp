#include "stdafx.h"
#include "FTSPI.h"

vector<CFTInterface*> ftInterfaces;

int InitFTInterface()
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
		FTHINFO fthinfo;
		sprintf_s(fthinfo.description, _countof(fthinfo.description), _T("%s(%s)"), devList[i].Description, devList[i].SerialNumber);
		fthinfo.index = i;
		fthinfo.rptr = fthinfo.wptr = 0;
		switch (devList[i].Type)
		{
		case FT_DEVICE_232H:
			ftInterfaces.push_back(new CFTSPI(fthinfo));
			break;
		case FT_DEVICE_2232C:
			ftInterfaces.push_back(new CFTHBE(fthinfo));
			break;
		}
	}
	delete[] devList;
	return (numDevices != 0);
}

FTHINFO& FTHINFO::operator=(FTHINFO& fth)
{
	ftHandle = fth.ftHandle;
	index = fth.index;
	rptr = fth.rptr;
	wptr = fth.wptr;
	memcpy_s(description, _countof(description), fth.description, _countof(fth.description));
	memcpy_s(cmdbuf, _countof(cmdbuf), fth.cmdbuf, _countof(fth.cmdbuf));
	return *this;
}

CFTInterface::CFTInterface()
{
	bValid = Init();
}

CFTInterface::~CFTInterface()
{
	FT_Close(FTChannel.ftHandle);
}

void CFTInterface::BufferPush(BYTE data)
{
	FTChannel.cmdbuf[FTChannel.wptr++] = data;
	if (FTChannel.wptr == (BUFSIZE - 1)) { BufferFlush(); }
}

void CFTInterface::BufferPush(BYTE* data, UINT32 length)
{
	if ((FTChannel.wptr + length) >= (BUFSIZE - 1)) { BufferFlush(); }
	memcpy(&FTChannel.cmdbuf[FTChannel.wptr], data, length);
	FTChannel.wptr += length;
}

FT_STATUS CFTInterface::BufferedRead(UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	return ret;
}

void CFTInterface::GetInterfaceDesc(TCHAR* str, int len)
{
	sprintf_s(str, len, _T("%s"), FTChannel.description);
}

FT_STATUS CFTInterface::BufferFlush()
{
	FT_STATUS ret = FT_OTHER_ERROR;
	DWORD written = 0;
	if (FTChannel.wptr) {
		//SPIChannel[index].cmdbuf[SPIChannel[index].wptr++] = 0x87;	//force flush
		ret = FT_Write(FTChannel.ftHandle, FTChannel.cmdbuf, FTChannel.wptr, &written);
		FTChannel.wptr = 0;
		//BYTE forceflush = 0x87;
		//ret = FT_Write(SPIChannel[index].ftHandle, &forceflush, 1, &written);
	}
	else {
		ret = FT_OK;
	}
	return ret;
}


CFTSPI::CFTSPI(FTHINFO& fth) : CFTInterface()
{
	FT_STATUS status = FT_OK;
	FTChannel = fth;
	status = FT_Open(FTChannel.index, &FTChannel.ftHandle);
	assert(status == FT_OK);
	status = Init();
	assert(status == FT_OK);
}

FT_STATUS CFTSPI::Init()
{
	FT_STATUS ret = FT_OTHER_ERROR;
	//ret = FT_SetBaudRate(SPIChannel[index].ftHandle, 2000000L);	//1Mbps
	//if (ret != FT_OK) return ret;
	ret = FT_Purge(FTChannel.ftHandle, FT_PURGE_RX | FT_PURGE_TX);
	if (ret != FT_OK) return ret;
	::Sleep(10);
	ret = FT_SetLatencyTimer(FTChannel.ftHandle, 1);	//1ms
	if (ret != FT_OK) return ret;
	ret = FT_SetBitMode(FTChannel.ftHandle, 0x0, 0x00);	//reset mode
	if (ret != FT_OK) return ret;
	::Sleep(10);
	ret = FT_SetBitMode(FTChannel.ftHandle, 0x00, 0x02);	//MPSSE mode
	if (ret != FT_OK) return ret;
	ret = FT_Purge(FTChannel.ftHandle, FT_PURGE_RX);
	if (ret != FT_OK) return ret;
	::Sleep(20);
	BufferPush(0x80);	//Set initial value of ADBUS
	BufferPush(0xf8);	//
	BufferPush(0xfb);	//
	BufferPush(0x82);	//Set initial value of ACBUS
	BufferPush(0xff);	//
	BufferPush(0xff);	//
	BufferPush(0x8a);	//disable initial devide
	BufferPush(0x86);	//CK devisor
	BufferPush(0x02);
	BufferPush(0x00);
	BufferPush(0x85);	//disable loopback
	BufferPush(0x8d);	//disable 3 phase clocking
	ret = BufferFlush();
	::Sleep(100);
	if (ret != FT_OK) return ret;
	return ret;
}


FT_STATUS CFTSPI::BufferedWrite(UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	UINT8 csmask = ~(1 << (cs + 3));
	if (sizeToTransfer) {
		UINT16 length = sizeToTransfer-1;
		BufferPush(0x80);	//assert CS
		BufferPush(0xf8 & csmask);	//
		BufferPush(0xfb);	//
		BufferPush(0x11);	//ve+ edge MSB first no read
		BufferPush((BYTE*)(&length), 2);	//length
		BufferPush(buffer, sizeToTransfer);
		BufferPush(0x80);	//dessert CS
		BufferPush(0xf8);	//
		BufferPush(0xfb);	//
		BufferFlush();
	}
	return ret;
}

FT_STATUS CFTSPI::FT_WriteGPIO( UINT8 dir, UINT8 value)
{
	FT_STATUS ret = FT_OK;
	BufferPush(0x82);	//Set high byte
	BufferPush(value);	//
	BufferPush(dir);	//
	BufferFlush();
	return ret;
}

void CFTSPI::InitialClear()
{
	BufferPush(0x82);	//Set high byte
	BufferPush(0x00);	//assert IC
	BufferPush(0xff);	//dir
	BufferFlush();
	::Sleep(10);
	BufferPush(0x82);	//Set high byte
	BufferPush(0xff);	//dessert IC
	BufferPush(0xff);	//dir
	BufferFlush();
}

CFTHBE::CFTHBE(FTHINFO& fth) : CFTInterface()
{
	FT_STATUS status = FT_OK;
	FTChannel = fth;
	status = FT_Open(FTChannel.index, &FTChannel.ftHandle);
	assert(status == FT_OK);
	status = Init();
	assert(status == FT_OK);
}

FT_STATUS CFTHBE::Init()
{
	FT_STATUS ret = FT_OTHER_ERROR;
	//ret = FT_SetBaudRate(SPIChannel[index].ftHandle, 2000000L);	//1Mbps
	//if (ret != FT_OK) return ret;
	ret = FT_Purge(FTChannel.ftHandle, FT_PURGE_RX | FT_PURGE_TX);
	if (ret != FT_OK) return ret;
	::Sleep(10);
	ret = FT_SetLatencyTimer(FTChannel.ftHandle, 1);	//1ms
	if (ret != FT_OK) return ret;
	ret = FT_SetBitMode(FTChannel.ftHandle, 0x0, 0x00);	//reset mode
	if (ret != FT_OK) return ret;
	::Sleep(10);
	ret = FT_SetBitMode(FTChannel.ftHandle, 0x00, 0x02);	//MPSSE mode
	if (ret != FT_OK) return ret;
	ret = FT_Purge(FTChannel.ftHandle, FT_PURGE_RX);
	if (ret != FT_OK) return ret;
	::Sleep(20);
	BufferPush(0x82);	//Set initial value of ACBUS
	BufferPush(0xff);	//
	BufferPush(0xff);	//
	BufferPush(0x86);	//CK devisor (6MHz)
	BufferPush(0x00);
	BufferPush(0x00);
	BufferPush(0x85);	//disable loopback
	BufferPush(0x8d);	//disable 3 phase clocking
	ret = BufferFlush();
	::Sleep(100);
	if (ret != FT_OK) return ret;
	return ret;
}


FT_STATUS CFTHBE::BufferedWrite(UINT8* buffer, UINT32 sizeToTransfer, UINT32 cs)
{
	FT_STATUS ret = FT_OK;
	UINT8 csmask = ~(1 << (cs + 3));
	if (sizeToTransfer) {
		UINT16 length = sizeToTransfer - 1;
		BufferPush(0x80);	//assert CS
		BufferPush(0xf8 & csmask);	//
		BufferPush(0xfb);	//
		BufferPush(0x11);	//ve+ edge MSB first no read
		BufferPush((BYTE*)(&length), 2);	//length
		BufferPush(buffer, sizeToTransfer);
		BufferPush(0x80);	//dessert CS
		BufferPush(0xf8);	//
		BufferPush(0xfb);	//
		BufferFlush();
	}
	return ret;
}

FT_STATUS CFTHBE::FT_WriteGPIO(UINT8 dir, UINT8 value)
{
	FT_STATUS ret = FT_OK;
	BufferPush(0x82);	//Set high byte
	BufferPush(value);	//
	BufferPush(dir);	//
	BufferFlush();
	return ret;
}

void CFTHBE::InitialClear()
{
	BufferPush(0x82);	//Set high byte
	BufferPush(0x00);	//assert IC
	BufferPush(0xff);	//dir
	BufferFlush();
	::Sleep(10);
	BufferPush(0x82);	//Set high byte
	BufferPush(0xff);	//dessert IC
	BufferPush(0xff);	//dir
	BufferFlush();
}

