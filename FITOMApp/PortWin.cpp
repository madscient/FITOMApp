#include "STDAFX.H"
#include "SCCIWrapper.h"
#include "Port.h"

CSCCIPort::CSCCIPort() : regsize(0), pInterface(0), pChip(0)
{
}

CSCCIPort::CSCCIPort(scciInterface* pif, SoundChip* pchip, size_t maxreg) : pInterface(pif), pChip(pchip), regsize(maxreg)
{
}

void CSCCIPort::write(uint16_t addr, uint16_t data)
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

uint8_t CSCCIPort::read(uint16_t addr)
{
	if (pChip && addr < regsize) {
		return uint8_t(pChip->getRegister(DWORD(addr)));
	}
	return 255;
}

uint8_t CSCCIPort::status()
{
	return 0;
}

void CSCCIPort::reset()
{
	if (pChip) {
		pChip->init();
#ifdef DEBUG
		TCHAR str[80];
		StringCchPrintf(str, _countof(str), _T("reset %08x\n"), physical_id);
		OutputDebugString(str);
#endif
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

void CSCCIPort::GetInterfaceDesc(TCHAR* str, int len)
{
	sprintf_s(str, len, _T("%s"), pInterface->sii->cInterfaceName);
}

int CSCCIPort::GetPanpot()
{
	SCCI_SOUND_CHIP_INFO* pci = pChip->getSoundChipInfo();
	return pci->dSoundLocation;
}

CFTPort::CFTPort() : pInterface(0), regsize(0), chidx(0), csidx(0), ftHandle(0)
{
}

CFTPort::CFTPort(CFTInterface* pif, uint32_t index, uint32_t cs, size_t maxreg)	: csidx(cs), chidx(index), pInterface(pif)
{
	//assert(pif && pif->IsValid() && index < pif->GetChannels() && cs < 5);
}

CFTPort::~CFTPort(void)
{
}

void CFTPort::write(uint16_t addr, uint16_t data)
{
	uint32_t sizeToTransfer = 0;
	uint32_t sizeTransfered = 0;
	FT_STATUS status;
	BYTE buf[2];

	buf[0] = BYTE(addr);
	buf[1] = BYTE(data);
	/* Write command EWEN(with CS_High -> CS_Low) */
	sizeToTransfer = 2;
	sizeTransfered = 0;
	status = pInterface->BufferedWrite(buf, sizeToTransfer, csidx);
	//pInterface->SPI_Flush(chidx);
#ifdef DEBUG
	TCHAR str[80];
	StringCchPrintf(str, _countof(str), _T("reg %08x %03x %02x\n"), physical_id, addr, data);
	OutputDebugString(str);
#endif
}

void CFTPort::writeBurst(uint16_t addr, BYTE* buf, size_t length)
{
	BYTE* newbuf = new BYTE[length + 1];
	newbuf[0] = BYTE(addr);
	memcpy(&newbuf[1], buf, length);
	writeBurst(newbuf, length + 1);
	delete[] newbuf;
}

void CFTPort::writeBurst(BYTE* buf, size_t length)
{
	FT_STATUS status = pInterface->BufferedWrite(buf, length, csidx);
	//pInterface->SPI_Flush(chidx);
	assert(status == FT_OK);
#ifdef DEBUG
	TCHAR str[80];
	StringCchPrintf(str, _countof(str), _T("reg %08x %03x burst %04x\n"), physical_id, length);
	OutputDebugString(str);
	for (size_t i = 0; i < length; i++) {
		StringCchPrintf(str, _countof(str), _T("%02x "), buf[i]);
		OutputDebugString(str);
	}
	OutputDebugString(_T("\n"));
#endif
}

uint8_t CFTPort::read(uint16_t addr)
{
	uint8_t ret = 0;
#ifdef DEBUG
	TCHAR str[80];
	StringCchPrintf(str, _countof(str), _T("read %08x %03x=%02x\n"), physical_id, addr, ret);
	OutputDebugString(str);
#endif
	return ret;
}

uint8_t CFTPort::status()
{
	return 0;
}

void CFTPort::flush()
{
	if (pInterface) {
		pInterface->BufferFlush();
#ifdef DEBUG
		TCHAR str[80];
		StringCchPrintf(str, _countof(str), _T("flush %08x\n"), physical_id);
		OutputDebugString(str);
#endif
	}
}

void CFTPort::reset()
{
	if (pInterface) {
		pInterface->FT_WriteGPIO(0xff, 0xff);
		::Sleep(2);
		pInterface->FT_WriteGPIO(0xff, 0x00);
		::Sleep(2);
		pInterface->FT_WriteGPIO(0xff, 0xff);
	}
#ifdef DEBUG
	TCHAR str[80];
	StringCchPrintf(str, _countof(str), _T("reset %08x\n"), physical_id);
	OutputDebugString(str);
#endif
}

int CFTPort::GetClock()
{
	return 0;
}

int CFTPort::GetDesc(TCHAR* str, int len)
{
	return sprintf_s(str, len, _T("FTSPI:%i:%i"), chidx, csidx);
}

int CFTPort::GetPanpot()
{
	return 0;
}

void CFTPort::GetInterfaceDesc(TCHAR* str, int len)
{
	pInterface->GetInterfaceDesc(str, len);
}

CDebugPort::CDebugPort() : chipname(0), regsize(0), regbak(0)
{
}

CDebugPort::CDebugPort(const TCHAR* name, size_t maxreg) : chipname(name), regsize(maxreg)
{
	regbak = new uint8_t[maxreg];
	::ZeroMemory(regbak, maxreg);
}

uint8_t CDebugPort::status()
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

void CDebugPort::write(uint16_t addr, uint16_t data)
{
	TCHAR str[80];
	StringCchPrintf(str, 80, _T("reg %s %03x %02x\n"), chipname, addr, data);
	OutputDebugString(str);
	regbak[addr] = uint8_t(data);
}

uint8_t CDebugPort::read(uint16_t addr)
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
