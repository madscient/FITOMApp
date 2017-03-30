#include "STDAFX.H"
#include "Port.h"

void CDblPort::write(UINT16 reg, UINT16 val, int v)
{
	(reg < 0x100) ? port[0]->write(reg, val, v) : port[1]->write(reg & 0xff, val, v);
}

UINT8 CDblPort::read(UINT16 reg, int v)
{
	return (reg < 0x100) ? port[0]->read(reg, v) : port[1]->read(reg & 0xff, v);
}

UINT8 CDblPort::status()
{
	return port[0]->status();
}

CPort* CDblPort::GetSubPort(int idx)
{
	return port[idx&1];
}

int CDblPort::GetDesc(TCHAR* str, int len)
{
	int off = port[0]->GetDesc(str, len);
	str[off] = _T('\n');
	int res = port[1]->GetDesc(&str[off+1], (len - off - 1));
	return strlen(str);
}
