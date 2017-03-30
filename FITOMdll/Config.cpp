#include "stdafx.h"
#include "FITOM.h"
#include "FITOMCfg.h"
#include "SoundDev.h"
#include "MIDI.h"

int CFITOM::ImportConfig(CFITOMConfig* cfg)
{
	int res = 0;
	theConfig = cfg;
	mpus = cfg->GetMidiInputs();
	for (int i = 0; i < mpus; i++) {
		midi[i] = new CMidiInst(cfg->GetMidiIn(i), this);
		if (midi[i]) {
			for (int j = 0; j < 16; j++) {
				int devid = cfg->GetChannelMapDevice(j);
				if (devid > 0) {
					if (devid == DEVICE_RHYTHM) {
						midi[i]->AssignRhythm(j);
					}
					else {
						CSoundDevice* psd = cfg->GetLogDeviceFromID(devid);
						psd = psd ? psd : cfg->GetLogDeviceFromIndex(0);
						midi[i]->AssignInst(j, psd, cfg->GetChannelMapPoly(j));
					}
				}
				else {
					midi[i]->AssignInst(j, NULL, 0);
				}
			}
		}
	}
	return res;
}
