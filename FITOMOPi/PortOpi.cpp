#include "stdafx.h"
#include "Port.h"
#include "SPFMUart.h"

CSPFMPort::CSPFMPort() : regsize(0), regbak(0)
{
}

CSPFMPort::CSPFMPort(CSPFMUart* puart, UINT32 slot, UINT32 addr, size_t maxreg)
	: spfm(puart), baseSlot(slot), baseAddr(addr), regsize(maxreg)
{
	regbak = new UINT8[maxreg];
	::ZeroMemory(regbak, maxreg);
}

UINT8 CSPFMPort::status()
{
	return 0;
}

int CSPFMPort::GetDesc(TCHAR* str, int len)
{
	size_t ret;
	std::sprintf(str, _T("SPFM:%08X"), physical_id);
	ret = strlen(str);
	return ++ret;
}

void CSPFMPort::write(UINT16 addr, UINT16 data, int v)
{
	TCHAR str[80];
	regbak[addr] = UINT8(data);
	spfm->write(baseSlot, (addr >> 7) & 0xe, data);
}

UINT8 CSPFMPort::read(UINT16 addr, int v)
{
	return (addr < regsize) ? regbak[addr] : 0;
}

void CSPFMPort::reset()
{
	TCHAR str[80];
	::ZeroMemory(regbak, regsize);
}
