#include "stdafx.h"
#include "PSGBase.h"

CPSGBase::CPSGBase(uint8_t devid, CPort* pt, int regsize, uint8_t ch, int fsamp)
	: CPSGBase(devid, pt, regsize, ch, fsamp, 1, -576, FnumTableType::TonePeriod)
	//	: CSoundDevice(devid, ch, fsamp, 1, -576, FnumTableType::TonePeriod, pt)
{
}

CPSGBase::CPSGBase(uint8_t devid, CPort* pt, int regsize, uint8_t ch, int fsamp, int div, int off, FnumTableType type)
	: CSoundDevice(devid, ch, fsamp, div, off, type, pt, regsize)
{
	ops = 0;
	lfoTL = new uint8_t[chs];
	egattr = new CEnvelope[chs];
}

ISoundDevice::FNUM CPSGBase::GetFnumber(uint8_t ch, int16_t offset)
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

	ret.block = uint8_t(oct);
	ret.fnum = Fnum[index];
	return ret;
}

void CPSGBase::UpdateKey(uint8_t ch, uint8_t keyon)
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

void CPSGBase::TimerCallBack(uint32_t tick)
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

