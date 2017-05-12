#include "STDAFX.H"
#include "Port.h"

//-----------------------------------------------------------
CMappedPort::CMappedPort(CPort* pt, UINT32 addr, UINT32 range) : CMappedPort()
{
	Map(pt, addr, range);
}

UINT32 CMappedPort::GetNextAddress()
{
	UINT32 ret = 0;
	for (int i = 0; i < ports.size(); i++) {
		ret = max(ret, (ports[i].addr + ports[i].range));
	}
	return ret;
}

UINT32 CMappedPort::Append(CPort* pt, UINT32 range)
{
	return Map(pt, GetNextAddress(), range);
}

UINT32 CMappedPort::Map(CPort* pt, UINT32 addr, UINT32 range)
{
	PORTMAP ptmap;
	ptmap.port = pt;
	ptmap.addr = addr;
	ptmap.range = range;
	ports.push_back(ptmap);
	return addr;
}

int CMappedPort::GetPortIndex(UINT32 addr)
{
	for (int i = 0; i < ports.size(); i++) {
		if (ports[i].addr <= addr && addr < (ports[i].addr + ports[i].range)) {
			return i;
		}
	}
	return -1;
}

CPort* CMappedPort::GetSubPort(UINT32 addr)
{
	int i = GetPortIndex(addr);
	if (i < 0 || i > ports.size()) {
		return 0;
	}
	return ports[i].port;
}

void CMappedPort::write(UINT16 addr, UINT16 data)
{
	int i = GetPortIndex(addr);
	if (i >= 0) {
		ports[i].port->write(addr - ports[i].addr, data);
	}
}

UINT8 CMappedPort::read(UINT16 addr)
{
	UINT8 ret = 0;
	int i = GetPortIndex(addr);
	if (i >= 0) {
		ret = ports[i].port->read(addr - ports[i].addr);
	}
	return ret;
}

