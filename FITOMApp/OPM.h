#ifndef __OPM_H__
#define __OPM_H__

#include "FITOMdefine.h"
#include "SoundDev.H"
#include "MIDI.h"

class COPM : public CSoundDevice {
protected:
	virtual FNUM GetFnumber(uint8_t ch, int16_t offset = 0);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateLFO() {};
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void ResetLFO(uint8_t ch, uint8_t lfonum) {};

	static uint8_t KeyCode[];
	static uint8_t map[];
	static uint8_t carmsk[];
	uint8_t lfos;
	LFORESOURCE* lfores;
public:
	COPM(CPort* pt, int fsamp);
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void SetLFOParam(FMVOICE* voice, int update) {};
	virtual void SetLFOMode(uint8_t ch, uint8_t mode, int update) {};
	virtual void EnableDevPM(uint8_t ch, uint8_t on);
	virtual void EnableDevAM(uint8_t ch, uint8_t on);
	virtual void SetDevPMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevAMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevPMRate(uint8_t ch, uint8_t rate);
	virtual void SetDevAMRate(uint8_t ch, uint8_t rate);
};

class COPP : public COPM {
protected:
	virtual void UpdateLFO() {};
public:
	COPP(CPort* pt, int fsamp);
	virtual void Init();
};

class COPZ : public COPM {
protected:
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
	uint8_t lfos;
public:
	COPZ(CPort* pt, int fsamp);
	virtual void Init();
};

#endif
