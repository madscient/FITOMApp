#ifndef __PORT_H__
#define __PORT_H__

#pragma once
#include <string>
#include <vector>

class CPort
{
protected:
	uint32_t physical_id;
public:
	CPort() {};
	~CPort(void) {};
	virtual void write(uint16_t addr, uint16_t data) = 0;
	virtual void writeRaw(uint16_t addr, uint16_t data) = 0;
	virtual void writeBurst(uint16_t addr, BYTE* data, size_t length);
	virtual void writeBurst(BYTE* data, size_t length) {};
	virtual uint8_t read(uint16_t addr) = 0;
	virtual uint8_t status() = 0;
	virtual void reset() = 0;
	virtual void flush() {};
	virtual std::string GetDesc() = 0;
	virtual int GetClock() = 0;
	virtual int GetPanpot() = 0;
	virtual std::string GetInterfaceDesc() = 0;
	virtual uint32_t GetPhysicalId() { return physical_id; };
	virtual void SetPhysicalId(uint32_t id) { physical_id = id; };
};

class CMultiPort : public CPort
{
public:
	virtual CPort* GetSubPort(int idx) = 0;
	virtual int GetPortCount() = 0;
};

class COffsetPort : public CPort
{
protected:
	uint16_t offset;
	CPort* parent;
public:
	COffsetPort() : offset(0), parent(0) {};
	COffsetPort(CPort* src, uint16_t off) : parent(src), offset(off) {};
	virtual void write(uint16_t addr, uint16_t data) { return parent ? parent->write(addr + offset, data) : void(0); };
	virtual void writeRaw(uint16_t addr, uint16_t data) { return parent ? parent->write(addr, data) : void(0); };
	virtual uint8_t read(uint16_t addr) { return parent ? parent->read(addr + offset) : 0; };
	virtual uint8_t status() { return parent ? parent->status() : 0; };
	virtual void reset() { parent ? parent->reset() : void(0); };
	virtual int GetClock() { return parent ? parent->GetClock() : 0; };
	virtual uint32_t GetPhysicalId() { return parent ? parent->GetPhysicalId() : 0; };
	virtual void SetPhysicalId(uint32_t id) { parent ? parent->SetPhysicalId(id) : void(0); };
	virtual int GetPanpot() { return parent ? (parent->GetPanpot()) : 0; };
	virtual std::string GetInterfaceDesc() { return parent->GetInterfaceDesc(); };
	virtual std::string GetDesc();
};

class CMappedPort : public CMultiPort
{
protected:
	struct PORTMAP {
		CPort* port;
		uint32_t addr;
		uint32_t range;
	};
	std::vector<PORTMAP> ports;
	CPort* GetSubPort(uint32_t addr);
	int GetPortIndex(uint32_t addr);
	uint32_t GetNextAddress();
public:
	CMappedPort() {};
	CMappedPort(CPort* pt, uint32_t addr, uint32_t range);
	~CMappedPort() {};
	uint32_t Append(CPort* pt, uint32_t size);
	uint32_t Map(CPort* pt, uint32_t addr, uint32_t size);
	virtual void write(uint16_t addr, uint16_t data);
	virtual void writeRaw(uint16_t addr, uint16_t data) { write(addr, data); };
	virtual uint8_t read(uint16_t addr);
	virtual uint8_t status() { return (0 < ports.size()) ? ports[0].port->status() : 0; };
	virtual void reset();
	virtual int GetPanpot() { return (0 < ports.size()) ? ports[0].port->GetPanpot() : 0; };
	virtual int GetClock() { return (0 < ports.size()) ? ports[0].port->GetClock() : 0; };
	virtual uint32_t GetPhysicalId() { return 0; };
	virtual CPort* GetSubPort(int idx) { return (idx < ports.size()) ? ports[idx].port : 0; };
	virtual int GetPortCount() { return ports.size(); };
	virtual std::string GetInterfaceDesc();
	virtual std::string GetDesc();
};

class CDebugPort : public CPort
{
protected:
	uint8_t* regbak;
	size_t regsize;
	const TCHAR* chipname;
public:
	CDebugPort();
	CDebugPort(const TCHAR* name, size_t maxreg);
	~CDebugPort(void) { delete[] regbak; };
	virtual void write(uint16_t addr, uint16_t data);
	virtual uint8_t read(uint16_t addr);
	virtual uint8_t status();
	virtual void reset();
	virtual std::string GetDesc();
};


#endif

