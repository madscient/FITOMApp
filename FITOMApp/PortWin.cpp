#include "STDAFX.H"
#include "FTInterface.h"
#include <boost/format.hpp>

CFT825Port::CFT825Port() : pInterface(0), regsize(0), chidx(0), csidx(0), ftHandle(0)
{
}

CFT825Port::CFT825Port(CFT232HSPI* pif, uint32_t index, uint32_t cs, size_t maxreg)	: csidx(cs), chidx(index), pInterface(pif)
{
	//assert(pif && pif->IsValid() && index < pif->GetChannels() && cs < 5);
}

CFT825Port::~CFT825Port(void)
{
}

void CFT825Port::write(uint16_t addr, uint16_t data)
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
	status = pInterface->BufferedWrite( buf, sizeToTransfer, csidx);
	//pInterface->SPI_Flush(chidx);
}

void CFT825Port::writeBurst(uint16_t addr, BYTE* buf, size_t length)
{
	BYTE* newbuf = new BYTE[length + 1];
	newbuf[0] = BYTE(addr);
	memcpy(&newbuf[1], buf, length);
	writeBurst(newbuf, length + 1);
	delete[] newbuf;
}

void CFT825Port::writeBurst(BYTE* buf, size_t length)
{
	FT_STATUS status = pInterface->BufferedWrite( buf, length, csidx);
	//pInterface->SPI_Flush(chidx);
	assert(status == FT_OK);
}

uint8_t CFT825Port::read(uint16_t addr)
{
	uint8_t ret = 0;
	return ret;
}

uint8_t CFT825Port::status()
{
	return 0;
}

void CFT825Port::flush()
{
	if (pInterface) {
		pInterface->BufferFlush();
	}
}

void CFT825Port::reset()
{
	if (pInterface) {
		pInterface->FT_WriteGPIO( 0xff, 0xff);
		::Sleep(2);
		pInterface->FT_WriteGPIO( 0xff, 0x00);
		::Sleep(2);
		pInterface->FT_WriteGPIO( 0xff, 0xff);
	}
}

int CFT825Port::GetClock()
{
	return 0;
}

std::string CFT825Port::GetDesc()
{
	return (boost::format("%s cs:%d") % pInterface->GetInterfaceDesc() % csidx).str();
}

int CFT825Port::GetPanpot()
{
	return 0;
}

std::string CFT825Port::GetInterfaceDesc()
{
	return pInterface->GetInterfaceDesc();
}

/// <summary>
/// CRebirthPort
/// </summary>
CRebirthPort::CRebirthPort() : pInterface(0), ctladdr(0), dataddr(0), slot_no(0), regsize(0), regwait(0), datwait(0)
{
}

CRebirthPort::CRebirthPort(CFT245Rebirth* pif, uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg)
	: pInterface(pif), ctladdr(addr1), dataddr(addr2), slot_no(slot), regsize(maxreg), regwait(0), datwait(0)
{
	//assert(pif && pif->IsValid() && index < pif->GetChannels() && cs < 5);
}

CRebirthPort::~CRebirthPort(void)
{
}

void CRebirthPort::write(uint16_t addr, uint16_t data)
{
	if (pInterface) {
		pInterface->Write(ctladdr, addr, regwait);	//register
		pInterface->Write(dataddr, data, datwait);	//data
	}
}

uint8_t CRebirthPort::status()
{
	return 0;
}

void CRebirthPort::reset()
{
	if (pInterface) {
		pInterface->InitialClear();
	}
}

int CRebirthPort::GetClock()
{
	return 0;
}

std::string CRebirthPort::GetDesc()
{
	if (pInterface) {
		return (boost::format("%s slot:%d ctl:%04x dat:%04x") % pInterface->GetInterfaceDesc() % slot_no % ctladdr % dataddr).str();
	}
	return std::string("");
}

int CRebirthPort::GetPanpot()
{
	return 0;
}

std::string CRebirthPort::GetInterfaceDesc()
{
	return (pInterface) ? pInterface->GetInterfaceDesc() : std::string("");
}


/// <summary>
/// CDebugPort
/// </summary>
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

std::string CDebugPort::GetDesc()
{
	return (boost::format("Debug port: physical_id=%08x") % physical_id).str();
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
