#include "STDAFX.H"
#include "Port.h"

CSCCIPort::CSCCIPort() : regsize(0), pInterface(0), pChip(0)
{
}

CSCCIPort::CSCCIPort(SoundInterface* pif, SoundChip* pchip, size_t maxreg) : pInterface(pif), pChip(pchip), regsize(maxreg)
{
}

void CSCCIPort::write(UINT16 addr, UINT16 data)
{
	if (pChip) {
		pChip->setRegister(DWORD(addr), DWORD(data));
#ifdef DEBUG
		TCHAR str[80];
		StringCchPrintf(str, _countof(str), _T("reg %08x %03x %02x\n"), physical_id, addr, data);
		OutputDebugString(str);
#endif
	}
}

UINT8 CSCCIPort::read(UINT16 addr)
{
	if (pChip && addr < regsize) {
		return UINT8(pChip->getRegister(DWORD(addr)));
	}
	return 255;
}

UINT8 CSCCIPort::status()
{
	return 0;
}

void CSCCIPort::reset()
{
	if (pChip) {
		pChip->init();
	}
}

int CSCCIPort::GetDesc(TCHAR* str, int len)
{
	size_t ret;
	SCCI_SOUND_CHIP_INFO* pci = pChip->getSoundChipInfo();
	StringCchPrintf(str, len, _T("SCCI:%08X"), physical_id);
	StringCchLength(str, STRSAFE_MAX_CCH, &ret);
	return ++ret;
}

int CSCCIPort::GetClock()
{
	SCCI_SOUND_CHIP_INFO* pci = pChip->getSoundChipInfo();
	return pci->dClock;
}


CFTSPIPort::CFTSPIPort() : pInterface(0), regsize(0), chidx(0), csidx(0), ftHandle(0)
{
}

CFTSPIPort::CFTSPIPort(CFTSPI* pif, UINT32 index, UINT32 cs, size_t maxreg)	: CFTSPIPort()
{
	if (pif && index < pif->GetChannels() && cs < 5) {
		pInterface = pif;
		csidx = cs;
		regsize = maxreg;
		chidx = pInterface->GetChannelIndex(index);
		ftHandle = pInterface->GetChannelHandle(index);
		ChannelConfig channelConf = { 0 };
		channelConf.ClockRate = 10000000;
		channelConf.LatencyTimer = 1;
		channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_ACTIVELOW | csidx;
		channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

		FT_STATUS status = pInterface->SPI_InitChannel(ftHandle, &channelConf);
		ASSERT(status == FT_OK);
	}
}

CFTSPIPort::~CFTSPIPort(void)
{
	if (pInterface) {
		pInterface->SPI_CloseChannel(ftHandle);
	}
}

void CFTSPIPort::write(UINT16 addr, UINT16 data)
{
	UINT32 sizeToTransfer = 0;
	UINT32 sizeTransfered = 0;
	FT_STATUS status;
	BYTE buf[2];

	buf[0] = BYTE(addr);
	buf[1] = BYTE(data);
	/* Write command EWEN(with CS_High -> CS_Low) */
	sizeToTransfer = 16;
	sizeTransfered = 0;
	status = pInterface->SPI_Write(ftHandle, buf, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#ifdef DEBUG
	TCHAR str[80];
	StringCchPrintf(str, _countof(str), _T("reg %08x %03x %02x\n"), physical_id, addr, data);
	OutputDebugString(str);
#endif
}

void CFTSPIPort::writeBurst(UINT16 addr, BYTE* buf, size_t length)
{
	UINT32 sizeToTransfer = 1;
	UINT32 sizeTransfered = 0;
	uint8 baddr = (uint8)addr;
	FT_STATUS status;

	/* Write command EWEN(with CS_High -> CS_Low) */
	sizeTransfered = 0;

	status = pInterface->SPI_Write(ftHandle, &baddr, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
#ifdef DEBUG
	{
		TCHAR str[80];
		StringCchPrintf(str, _countof(str), _T("reg %08x %03x burst:%04x\n["), physical_id, addr, length);
		OutputDebugString(str);
	}
#endif
	for (uint32 i = 0; i < (length - 1); i++) {
		status = pInterface->SPI_Write(ftHandle, &buf[i], sizeToTransfer, &sizeTransfered,
			SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
#ifdef DEBUG
		{
			TCHAR str[8];
			StringCchPrintf(str, _countof(str), _T("%02x"), buf[i]);
			OutputDebugString(str);
		}
#endif
	}
	status = pInterface->SPI_Write(ftHandle, &buf[length - 1], sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#ifdef DEBUG
	{
		TCHAR str[8];
		StringCchPrintf(str, _countof(str), _T("%02x]\n"), buf[length - 1]);
		OutputDebugString(str);
	}
#endif
}

UINT8 CFTSPIPort::read(UINT16 addr)
{
	uint8 ret = 0xff;
	UINT32 sizeToTransfer = 1;
	UINT32 sizeTransfered = 0;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	uint8 buf = 0x80 | addr;
	FT_STATUS status;
	status = pInterface->SPI_Write(ftHandle, &buf, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	status = pInterface->SPI_Read(ftHandle, &ret, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	return ret;
}

UINT8 CFTSPIPort::status()
{
	return 0;
}

void CFTSPIPort::reset()
{
	pInterface->FT_WriteGPIO(ftHandle, 0xff, 0xff);
	pInterface->FT_WriteGPIO(ftHandle, 0xff, 0x00);
	::Sleep(1);
	pInterface->FT_WriteGPIO(ftHandle, 0xff, 0xff);
}

int CFTSPIPort::GetClock()
{
	return 0;
}

int CFTSPIPort::GetDesc(TCHAR* str, int len)
{
	return sprintf_s(str, len, _T("FTSPI:%i:%i"), chidx, csidx);
}

CDebugPort::CDebugPort() : chipname(0), regsize(0), regbak(0)
{
}

CDebugPort::CDebugPort(const TCHAR* name, size_t maxreg) : chipname(name), regsize(maxreg)
{
	regbak = new UINT8[maxreg];
	::ZeroMemory(regbak, maxreg);
}

UINT8 CDebugPort::status()
{
	return 0;
}

int CDebugPort::GetDesc(TCHAR* str, int len)
{
	size_t ret;
	StringCchPrintf(str, len, _T("Debug:%08X"), physical_id);
	StringCchLength(str, STRSAFE_MAX_CCH, &ret);
	return ++ret;
}

void CDebugPort::write(UINT16 addr, UINT16 data)
{
	TCHAR str[80];
	StringCchPrintf(str, 80, _T("reg %s %03x %02x\n"), chipname, addr, data);
	OutputDebugString(str);
	regbak[addr] = UINT8(data);
}

UINT8 CDebugPort::read(UINT16 addr)
{
	return (addr < regsize) ? regbak[addr] : 0;
}

void CDebugPort::reset()
{
	TCHAR str[80];
	StringCchPrintf(str, 80, _T("reset %s\n"), chipname);
	OutputDebugString(str);
	::ZeroMemory(regbak, regsize);
}