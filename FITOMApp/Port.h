#ifndef __PORT_H__
#define __PORT_H__

#ifdef _WIN32
#include "scci.h"
#include "SCCIDefines.h"
#endif

class CPort
{
protected:
	UINT32 physical_id;
public:
	CPort() {};
	~CPort(void) {};
	virtual void write(UINT16 addr, UINT16 data, int v) = 0;
	virtual UINT8 read(UINT16 addr, int v) = 0;
	virtual UINT8 status() = 0;
	virtual void reset() = 0;
	virtual int GetDesc(TCHAR* str, int len) = 0;
	virtual int GetClock() = 0;
	virtual UINT32 GetPhysicalId() { return physical_id; };
	virtual void SetPhysicalId(UINT32 id) { physical_id = id; };
};


class CDblPort : public CPort
{
public:
	CDblPort(CPort* pta, CPort* ptb);
	~CDblPort() {};
	virtual void write(UINT16 addr, UINT16 data, int v);
	virtual UINT8 read(UINT16 addr, int v);
	virtual UINT8 status();
	virtual void reset() { if (port[0]) port[0]->reset(); if (port[1]) port[1]->reset(); };
	virtual int GetDesc(TCHAR* str, int len);
	virtual int GetClock() { return (port[0]) ? port[0]->GetClock() : 0; };
	virtual UINT32 GetPhysicalId() { return port[0] ? port[0]->GetPhysicalId() : 0; };
	CPort* GetSubPort(int idx);
protected:
	CPort* port[2];
};

class COffsetPort : public CPort
{
protected:
	UINT16 offset;
	CPort* parent;
public:
	COffsetPort() : offset(0), parent(0) {};
	COffsetPort(CPort* src, UINT16 off) : parent(src), offset(off) {};
	virtual void write(UINT16 addr, UINT16 data, int v) { return parent ? parent->write(addr + offset, data, v) : void(0); };
	virtual UINT8 read(UINT16 addr, int v) { return parent ? parent->read(addr + offset, v) : 0; };
	virtual UINT8 status() { return parent ? parent->status() : 0; };
	virtual void reset() { parent ? parent->reset() : void(0); };
	virtual int GetClock() { return parent ? parent->GetClock() : 0; };
	virtual int GetDesc(TCHAR* str, int len) { return parent ? parent->GetDesc(str, len) : (str[0] = '\0'); };
	virtual UINT32 GetPhysicalId() { return parent ? parent->GetPhysicalId() : 0; };
	virtual void SetPhysicalId(UINT32 id) { parent ? parent->SetPhysicalId(id) : void(0); };
};

#ifdef _WIN32
class CSCCIPort : public CPort
{
protected:
	SoundChip* pChip;
	SoundInterface* pInterface;
	UINT8* regbak;
	size_t regsize;
public:
	CSCCIPort();
	CSCCIPort(SoundInterface* pif, SoundChip* pchip, size_t maxreg);
	~CSCCIPort(void) { delete[] regbak; };
	virtual void write(UINT16 addr, UINT16 data, int v);
	virtual UINT8 read(UINT16 addr, int v);
	virtual UINT8 status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetDesc(TCHAR* str, int len);
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
	virtual void write(UINT16 addr, UINT16 data, int v);
	virtual UINT8 read(UINT16 addr, int v);
	virtual UINT8 status();
	virtual void reset();
	virtual int GetDesc(TCHAR* str, int len);
};


#endif

