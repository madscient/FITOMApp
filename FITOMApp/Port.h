#ifndef __PORT_H__
#define __PORT_H__

#ifdef _WIN32
#include "scci.h"
#include "SCCIDefines.h"
#endif

#include <vector>

class CPort
{
protected:
	UINT32 physical_id;
public:
	CPort() {};
	~CPort(void) {};
	virtual void write(UINT16 addr, UINT16 data) = 0;
	virtual void writeRaw(UINT16 addr, UINT16 data) = 0;
	virtual UINT8 read(UINT16 addr) = 0;
	virtual UINT8 status() = 0;
	virtual void reset() = 0;
	virtual int GetDesc(TCHAR* str, int len) = 0;
	virtual int GetClock() = 0;
	virtual UINT32 GetPhysicalId() { return physical_id; };
	virtual void SetPhysicalId(UINT32 id) { physical_id = id; };
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
	UINT16 offset;
	CPort* parent;
public:
	COffsetPort() : offset(0), parent(0) {};
	COffsetPort(CPort* src, UINT16 off) : parent(src), offset(off) {};
	virtual void write(UINT16 addr, UINT16 data) { return parent ? parent->write(addr + offset, data) : void(0); };
	virtual void writeRaw(UINT16 addr, UINT16 data) { return parent ? parent->write(addr, data) : void(0); };
	virtual UINT8 read(UINT16 addr) { return parent ? parent->read(addr + offset) : 0; };
	virtual UINT8 status() { return parent ? parent->status() : 0; };
	virtual void reset() { parent ? parent->reset() : void(0); };
	virtual int GetClock() { return parent ? parent->GetClock() : 0; };
	virtual int GetDesc(TCHAR* str, int len) { return parent ? parent->GetDesc(str, len) : (str[0] = '\0'); };
	virtual UINT32 GetPhysicalId() { return parent ? parent->GetPhysicalId() : 0; };
	virtual void SetPhysicalId(UINT32 id) { parent ? parent->SetPhysicalId(id) : void(0); };
};

class CMappedPort : public CMultiPort
{
protected:
	struct PORTMAP {
		CPort* port;
		UINT32 addr;
		UINT32 range;
	};
	std::vector<PORTMAP> ports;
	CPort* GetSubPort(UINT32 addr);
	int GetPortIndex(UINT32 addr);
	UINT32 GetNextAddress();
public:
	CMappedPort() {};
	CMappedPort(CPort* pt, UINT32 addr, UINT32 range);
	~CMappedPort() {};
	UINT32 Append(CPort* pt, UINT32 size);
	UINT32 Map(CPort* pt, UINT32 addr, UINT32 size);
	virtual void write(UINT16 addr, UINT16 data);
	virtual void writeRaw(UINT16 addr, UINT16 data) { write(addr, data); };
	virtual UINT8 read(UINT16 addr);
	virtual UINT8 status() { return (0 < ports.size()) ? ports[0].port->status() : 0; };
	virtual void reset();
	virtual int GetDesc(TCHAR* str, int len);
	virtual int GetClock() { return (0 < ports.size()) ? ports[0].port->GetClock() : 0; };
	virtual UINT32 GetPhysicalId() { return 0; };
	virtual CPort* GetSubPort(int idx) { return (idx < ports.size()) ? ports[idx].port : 0; };
	virtual int GetPortCount() { return ports.size(); };
};

#ifdef _WIN32
class CSCCIPort : public CPort
{
protected:
	SoundChip* pChip;
	SoundInterface* pInterface;
	size_t regsize;
public:
	CSCCIPort();
	CSCCIPort(SoundInterface* pif, SoundChip* pchip, size_t maxreg);
	~CSCCIPort(void) {};
	virtual void write(UINT16 addr, UINT16 data);
	virtual void writeRaw(UINT16 addr, UINT16 data) { write(addr, data); };
	virtual UINT8 read(UINT16 addr);
	virtual UINT8 status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetDesc(TCHAR* str, int len);
};

class CFTSPIPort : public CPort
{
};

#endif

#ifdef _LINUX
class CSPFMUart;
class CSPFMPort : public CPort
{
protected:
	CSPFMUart* spfm;
	UINT32 baseSlot;
	UINT32 baseAddr;
	UINT8* regbak;
	size_t regsize;
public:
	CSPFMPort();
	CSPFMPort(CSPFMUart* puart, UINT32 slot, UINT32 addr, size_t maxreg);
	~CSPFMPort(void) { delete[] regbak; };
	virtual void write(UINT16 addr, UINT16 data, int v);
	virtual UINT8 read(UINT16 addr, int v);
	virtual UINT8 status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetDesc(TCHAR* str, int len);
};
#endif

class CDebugPort : public CPort
{
protected:
	UINT8* regbak;
	size_t regsize;
	const TCHAR* chipname;
public:
	CDebugPort();
	CDebugPort(const TCHAR* name, size_t maxreg);
	~CDebugPort(void) { delete[] regbak; };
	virtual void write(UINT16 addr, UINT16 data);
	virtual UINT8 read(UINT16 addr);
	virtual UINT8 status();
	virtual void reset();
	virtual int GetDesc(TCHAR* str, int len);
};


#endif

