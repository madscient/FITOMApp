#include "stdafx.h"
#include "PSGBase.h"

CPSGBase::CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp)
	: CPSGBase(devid, pt, regsize, ch, fsamp, 1, -576, FnumTableType::TonePeriod)
	//	: CSoundDevice(devid, ch, fsamp, 1, -576, FnumTableType::TonePeriod, pt)
{
}

CPSGBase::CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp, int div, int off, FnumTableType type)
	: CSoundDevice(devid, ch, fsamp, div, off, type, pt, regsize)
{
	ops = 0;
	lfoTL = new UINT8[chs];
	egattr = new CEnvelope[chs];
}

ISoundDevice::FNUM CPSGBase::GetFnumber(UINT8 ch, SINT16 offset)
{
	FNUM ret;
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	int oct = (-NoteOffset / 12) - 1;

	// normalize
	while (index < 0) {
		oct--;
		index += 768;
	}
	while (index >= 768) {
		oct++;
		index -= 768;
	}

	ret.block = UINT8(oct);
	ret.fnum = Fnum[index];
	return ret;
}

void CPSGBase::UpdateKey(UINT8 ch, UINT8 keyon)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	if (keyon) {
		//UpdateVoice(ch);
		egattr[ch].Start(&voice->op[0]);
	}
	else {
		egattr[ch].Release();
	}
}

void CPSGBase::PollingCallBack()
{
}

void CPSGBase::UpdateLevelEG()
{
	for (int i = 0; i<chs; i++) {
		if (egattr[i].GetPhase() != CEnvelope::EG_NONE) {
			egattr[i].Update();
		}
		else if (GetChAttribute(i)->IsRunning()) {
			EGOff(i);
		}
	}
}

void CPSGBase::TimerCallBack(UINT32 tick)
{
	if (/*(tick & 1)*/1) {
		UpdateLevelEG();
	}
	CSoundDevice::TimerCallBack(tick);

	for (int i = 0; i<chs; i++) {
		if (egattr[i].GetPhase() != CEnvelope::EG_NONE) {
			UpdateVolExp(i);
		}
	}
}

