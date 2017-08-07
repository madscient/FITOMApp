#include "stdafx.h"
#include "MAx.h"

CSD1::CSD1(CPort* pt, int fsamp)
	: CSoundDevice(DEVICE_SD1, 16, fsamp, 128, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x80)
{

}

