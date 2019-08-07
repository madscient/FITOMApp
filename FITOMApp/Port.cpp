#include "STDAFX.H"
#include "Port.h"

void CPort::writeBurst(uint16_t addr, BYTE* data, size_t length)
{
	for (size_t i = 0; i < length; i++) {
		writeRaw(addr, data[i]);
	}
}

//-----------------------------------------------------------
CMappedPort::CMappedPort(CPort* pt, uint32_t addr, uint32_t range) : CMappedPort()
{
	Map(pt, addr, range);
}

uint32_t CMappedPort::GetNextAddress()
{
	uint32_t ret = 0;
	for (int i = 0; i < ports.size(); i++) {
		ret = max(ret, (ports[i].addr + ports[i].range));
	}
	return ret;
}

uint32_t CMappedPort::Append(CPort* pt, uint32_t range)
{
	return Map(pt, GetNextAddress(), range);
}

uint32_t CMappedPort::Map(CPort* pt, uint32_t addr, uint32_t range)
{
	PORTMAP ptmap;
	ptmap.port = pt;
	ptmap.addr = addr;
	ptmap.range = range;
	ports.push_back(ptmap);
	return addr;
}

int CMappedPort::GetPortIndex(uint32_t addr)
{
	for (int i = 0; i < ports.size(); i++) {
		if (ports[i].addr <= addr && addr < (ports[i].addr + ports[i].range)) {
			return i;
		}
	}
	return -1;
}

CPort* CMappedPort::GetSubPort(uint32_t addr)
{
	int i = GetPortIndex(addr);
	if (i < 0 || i > ports.size()) {
		return 0;
	}
	return ports[i].port;
}

void CMappedPort::write(uint16_t addr, uint16_t data)
{
	int i = GetPortIndex(addr);
	if (i >= 0) {
		ports[i].port->write(addr - ports[i].addr, data);
	}
}

uint8_t CMappedPort::read(uint16_t addr)
{
	uint8_t ret = 0;
	int i = GetPortIndex(addr);
	if (i >= 0) {
		ret = ports[i].port->read(addr - ports[i].addr);
	}
	return ret;
}

void CMappedPort::reset()
{
	for (int i = 0; i < ports.size(); i++) {
		ports[i].port->reset();
	}
}

int CMappedPort::GetDesc(TCHAR* str, int len)
{
	tcsncpy(str, _T("Mapped Port"), len);
	return tcslen(str);
}
