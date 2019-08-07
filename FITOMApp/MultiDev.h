#ifndef __MULTIDEV_H__
#define __MULTIDEV_H__

#include "SoundDev.h"

class CMultiDevice : public CSoundDevice {
protected:
	int numchips;
	CSoundDevicePtr* chips;
	//virtual void UpdateKey(uint8_t ch, uint8_t keyon) {};
public:
	//CMultiDevice();
	virtual void Init();
	virtual void AddDevice(CSoundDevice* chip);
	CMultiDevice(CSoundDevice* chip1, CSoundDevice* chip2);
	virtual void SetMasterVolume(uint8_t vol, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void PollingCallBack();
	virtual void TimerCallBack(uint32_t tick);
	virtual void SetReg(uint16_t reg, uint8_t data, uint8_t v=1);
	virtual uint8_t GetReg(uint16_t reg, uint8_t v);
	virtual void Reset();
	virtual uint8_t GetChipCount() { return numchips; };
	virtual CPort* GetDevPort() { return numchips ? chips[0]->GetDevPort() : 0; };
};

class CSpanDevice : public CMultiDevice {
protected:
	struct DEVCHRES {
		int dev;
		int ch;
	};
	DEVCHRES* chres;
	int prior_dev;
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateFnumber(uint8_t ch, int update=1);
public:
	CSpanDevice(CSoundDevice* chip1, CSoundDevice* chips);
	//CSpanDevice();
	virtual CHATTR* GetChAttribute(uint8_t ch) const;
	virtual void AddDevice(CSoundDevice* chip);
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
};

class CUnison : public CMultiDevice {
protected:
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateFnumber(uint8_t ch, int update=1);
public:
	CUnison(CSoundDevice* chip1, CSoundDevice* chips);

	virtual void NoteOn(uint8_t ch, uint8_t vel=127);
	virtual void NoteOff(uint8_t ch);

	// Device status
	virtual void EnableCh(uint8_t ch, uint8_t ena);
	virtual uint8_t AllocCh(CMidiCh* parent, FMVOICE* voice = 0);
	virtual uint8_t Assign(uint8_t ch, CMidiCh* parent, FMVOICE* voice);
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void ReleaseCh(uint8_t ch);
	virtual uint8_t GetChs();
	virtual uint8_t GetAvailable();
	virtual uint8_t GetAssignable();
	virtual void Reset();

	// Physical accesses
	virtual void SetReg(uint16_t reg, uint8_t data, uint8_t v=1);
	virtual uint8_t GetReg(uint16_t reg, uint8_t v);

	// Ch Property settings
	virtual void SetSustain(uint8_t ch, uint8_t sus, int update=1);
	virtual void SetNoteFine(uint8_t ch, uint8_t note, int16_t fine, int update=1);
	virtual void SetVoice(uint8_t ch, FMVOICE* voice, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void SetVolume(uint8_t ch, uint8_t vol, int update=1);
	virtual void SetVelocity(uint8_t ch, uint8_t vel, int update=1);
	virtual void SetExpress(uint8_t ch, uint8_t exp, int update=1);
	virtual void SetPanpot(uint8_t ch, uint8_t pan, int update=1);
	virtual void SetMasterVolume(uint8_t vol, int update=1);
	virtual void SetLFOParam(FMVOICE* voice, int update=1);
	virtual void SetLFOMode(uint8_t ch, uint8_t mode, int update=1);

	virtual void EnableDevPM(uint8_t ch, uint8_t on);
	virtual void EnableDevAM(uint8_t ch, uint8_t on);
	virtual void SetDevPMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevAMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevPMRate(uint8_t ch, uint8_t rate);
	virtual void SetDevAMRate(uint8_t ch, uint8_t rate);

	//For Monitoring
	virtual uint32_t GetCurrentVoiceID(uint8_t ch) const;
	virtual FMVOICE* GetCurrentVoice(uint8_t ch) const;
	virtual uint8_t GetCurrentNote(uint8_t ch) const;
	virtual FNUM GetCurrentFnum(uint8_t ch) const;
	virtual void GetDescriptor(TCHAR* str, size_t len);
	virtual CHATTR* GetChAttribute(uint8_t ch) const { return chips[0]->GetChAttribute(ch); };

	//Callback
	virtual void PollingCallBack();
};

class CSplitPan : public CUnison {
protected:
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
public:
	CSplitPan(CSoundDevice* chip1, CSoundDevice* chips);
};

class CLinearPan : public CUnison {
protected:
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateVolExp(uint8_t ch);
public:
	CLinearPan(CSoundDevice* chip1, CSoundDevice* chips);
	virtual void SetVolume(uint8_t ch, uint8_t vol, int update=1);
	virtual int GetLocation() { return 3; };
};

#endif
