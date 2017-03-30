#include "stdafx.h"
#include "commands.h"

int showcmd(vector<string>& param)
{
	int showinterfacescmd(vector<string>& param);
	int showinterfacecmd(vector<string>& param);
	int showdevicescmd(vector<string>& param);
	int showdevicecmd(vector<string>& param);
	int showregistercmd(vector<string>& param);
	int showconfigcmd(vector<string>& param);
	int showversioncmd(vector<string>& param);
	int showdelaycmd(vector<string>& param);
	static const cmdvect showfuncvector[] = {
		{"interfaces", showinterfacescmd, },
		{"interface", showinterfacecmd, },
		{"devices", showdevicescmd, },
		{"device", showdevicecmd, },
		{"register", showregistercmd, },
		{"config", showconfigcmd, },
		{"version", showversioncmd, },
		{"delay", showdelaycmd, },
		{0, 0, },
	};
	if (param.size()) {
		string cmd = shift(param);
		for (int i=0; showfuncvector[i].cmd; i++) {
			if (cmd.compare(showfuncvector[i].cmd) == 0) {
				return showfuncvector[i].func(param);
			}
		}
	} else {
		cout << "show <";
		for (int i=0; showfuncvector[i].cmd; i++) {
			cout << showfuncvector[i].cmd << "|";
		}
		cout << ">" << endl << endl;
	}

	return 1;
}

int showinterfacescmd(vector<string>& param)
{
	if (pManager) {
		int numofif = pManager->getInterfaceCount();
		cout << "Interfaces: " << numofif << endl;
		for (int i=0; i<numofif; i++) {
			SCCI_INTERFACE_INFO* sifinfo = pManager->getInterfaceInfo(i);
			cout << i << ": " << sifinfo->cInterfaceName << ": " << sifinfo->iSoundChipCount << "devices" << endl << endl;
		}
	}
	return 1;
}

int showinterfacecmd(vector<string>& param)
{
	int st, en, target;
	st = en = target = -1;
	if (param.size()) {
		if (param.front().compare("all") == 0) {
			target = -1;
		} else {
			target = parseAsDec(param.front());
		}
	}
	if (pManager) {
		if (target >= 0) {
			st = target; en = st+1;
		} else {
			st = 0; en = pManager->getInterfaceCount();
		}
		for (int i=st; i<en; i++) {
			SCCI_INTERFACE_INFO* sifinfo = pManager->getInterfaceInfo(i);
			SoundInterface* psif = getSoundInterface(i);
			if (sifinfo && psif) {
				cout << "Interface No. : " << i << endl;
				cout << "Interface name: " << sifinfo->cInterfaceName << endl;
				cout << "Num of devices: " << sifinfo->iSoundChipCount << endl;
				cout << "Low level API : " << (psif->isSupportLowLevelApi() ? "Yes" : "No") << endl;
				cout << "Delay time(ms): " << psif->getDelay() << endl << endl;
				//pManager->releaseInterface(psif);
			}
		}
	}
	return 1;
}

int showregistercmd(vector<string>& param)
{
	return 1;
}

int showconfigcmd(vector<string>& param)
{
	return 1;
}

int showdevicescmd(vector<string>& param)
{
	int st, en, target;
	st = en = target = -1;
	if (param.size()) {
		if (param.front().compare("all") == 0) {
			target = -1;
		} else {
			target = parseAsDec(param.front());
		}
	}
	if (pManager) {
		if (target >= 0) {
			st = target; en = st+1;
		} else {
			st = 0; en = pManager->getInterfaceCount();
		}
		for (int i=st; i<en; i++) {
			SCCI_INTERFACE_INFO* sifinfo = pManager->getInterfaceInfo(i);
			SoundInterface* psif = getSoundInterface(i);
			if (sifinfo && psif) {
				cout << "Interface " << i << ": " << sifinfo->cInterfaceName << endl;
				for (int j=0; j<psif->getSoundChipCount(); j++) {
					SoundChip* psc = getSoundChipFromId(i, j);
					if (psc) {
						printSoundChipInfo(psc->getSoundChipInfo());
						//pManager->releaseSoundChip(psc);
					}
				}
				cout << endl;
			}
			//pManager->releaseInterface(psif);
		}
	}
	return 1;
}

int showdevicecmd(vector<string>& param)
{
	if (param.size()) {
		string chipname = shift(param);
		if (chipname.find(":") >= 0) {
			SoundChip* psc = getSoundChipFromString(chipname);
			if (psc) {
				printSoundChipInfo(psc->getSoundChipInfo());
				//pManager->releaseSoundChip(psc);
			}
		} else {
			SoundChip* psc = 0;
			while ((psc=getSoundChipFromString(chipname))) {
				printSoundChipInfo(psc->getSoundChipInfo());
			}
		}
		//pManager->releaseAllSoundChip();
		cout << endl;
	}else {
		cout << "Invalid parameter" <<endl << endl;
	}
	return 1;
}

int showversioncmd(vector<string>& param)
{
	if (pManager) {
		DWORD ver = 0;
		pManager->getVersion(&ver);
		cout << "SCCI Version: " << ver << endl << endl;
	}
	return 1;
}

int showdelaycmd(vector<string>& param)
{
	if (pManager) {
		cout << "Global: " << pManager->getDelay() << endl;
		for (int i=0; i<pManager->getInterfaceCount(); i++) {
			SoundInterface* psi = getSoundInterface(i);
			if (psi) {
				cout << "Interface" << i << ": " << psi->getDelay() << endl;
				//pManager->releaseInterface(psi);
			}
		}
		cout << endl;
	}
	return 1;
}
