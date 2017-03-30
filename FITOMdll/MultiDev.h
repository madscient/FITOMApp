#ifndef __MULTIDEV_H__
#define __MULTIDEV_H__

#include "SoundDev.h"

class CMultiDevice : public CSoundDevice {
protected:
	int numchips;
	CSoundDevicePtr* chips;
	//virtual void UpdateKey(UINT8 ch, UINT8 keyon) {};
public:
	//CMultiDevice();
	virtual void AddDevice(CSoundDevice* chip);
	CMultiDevice(CSoundDevice* chip1, CSoundDevice* chip2);
	virtual void SetMasterVolume(UINT8 vol, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void PollingCallBack();
	virtual void TimerCallBack(UINT32 tick);
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v=1);
	virtual UINT8 GetReg(UINT16 reg, UINT8 v);
	virtual void Reset();
	virtual UINT8 GetChipCount() { return numchips; };
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
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateFnumber(UINT8 ch, int update=1);
public:
	CSpanDevice(CSoundDevice* chip1, CSoundDevice* chips);
	//CSpanDevice();
	virtual CHATTR* GetChAttribute(UINT8 ch) const;
	virtual void AddDevice(CSoundDevice* chip);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan=0, FMVOICE* rv=0, FNUM* fnum=0);
};

class CUnison : public CMultiDevice {
protected:
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateFnumber(UINT8 ch, int update=1);
public:
	CUnison(CSoundDevice* chip1, CSoundDevice* chips);

	virtual void NoteOn(UINT8 ch, UINT8 vel=127);
	virtual void NoteOff(UINT8 ch);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);

	// Device status
	virtual void EnableCh(UINT8 ch, UINT8 ena);
	virtual UINT8 AllocCh(CMidiCh* parent, FMVOICE* voice = 0);
	virtual UINT8 Assign(UINT8 ch, CMidiCh* parent, FMVOICE* voice);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void ReleaseCh(UINT8 ch);
	virtual UINT8 GetChs();
	virtual UINT8 GetAvailable();
	virtual UINT8 GetAssignable();
	virtual void Reset();
	virtual UINT8 GetRhythmCaps();

	// Physical accesses
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v=1);
	virtual UINT8 GetReg(UINT16 reg, UINT8 v);

	// Ch Property settings
	virtual void SetSustain(UINT8 ch, UINT8 sus, int update=1);
	virtual void SetNoteFine(UINT8 ch, UINT8 note, SINT16 fine, int update=1);
	virtual void SetVoice(UINT8 ch, FMVOICE* voice, int update=1);
	virtual void ResetVoice(CMidiCh* pch, FMVOICE* voice, int update=1);
	virtual void SetVolume(UINT8 ch, UINT8 vol, int update=1);
	virtual void SetVelocity(UINT8 ch, UINT8 vel, int update=1);
	virtual void SetExpress(UINT8 ch, UINT8 exp, int update=1);
	virtual void SetPanpot(UINT8 ch, UINT8 pan, int update=1);
	virtual void SetMasterVolume(UINT8 vol, int update=1);
	virtual void SetLFOParam(FMVOICE* voice, int update=1);
	virtual void SetLFOMode(UINT8 ch, UINT8 mode, int update=1);
	virtual void SetRhythmVol(UINT8 vol, int update=1);

	virtual void EnableDevPM(UINT8 ch, UINT8 on);
	virtual void EnableDevAM(UINT8 ch, UINT8 on);
	virtual void SetDevPMDepth(UINT8 ch, UINT8 dep);
	virtual void SetDevAMDepth(UINT8 ch, UINT8 dep);
	virtual void SetDevPMRate(UINT8 ch, UINT8 rate);
	virtual void SetDevAMRate(UINT8 ch, UINT8 rate);

	//For Monitoring
	virtual UINT32 GetCurrentVoiceID(UINT8 ch) const;
	virtual FMVOICE* GetCurrentVoice(UINT8 ch) const;
	virtual UINT8 GetCurrentNote(UINT8 ch) const;
	virtual FNUM GetCurrentFnum(UINT8 ch) const;
	virtual void GetDescriptor(TCHAR* str, size_t len);
	virtual CHATTR* GetChAttribute(UINT8 ch) const { return chips[0]->GetChAttribute(ch); };

	//Callback
	virtual void PollingCallBack();
};

class CSplitPan : public CUnison {
protected:
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	CSplitPan(CSoundDevice* chip1, CSoundDevice* chips);
};

class CLinearPan : public CUnison {
protected:
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateVolExp(UINT8 ch);
public:
	CLinearPan(CSoundDevice* chip1, CSoundDevice* chips);
	virtual void SetVolume(UINT8 ch, UINT8 vol, int update=1);
};

#endif
