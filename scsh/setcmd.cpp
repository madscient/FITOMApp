#include "stdafx.h"
#include "commands.h"

int setcmd(vector<string>& param)
{
	int setdelaycmd(vector<string>& param);
	int setmodecmd(vector<string>& param);
	int setaquisitioncmd(vector<string>& param);
	int setcloclrangecmd(vector<string>& param);
	static const cmdvect setfuncvector[] = {
		{"delay", setdelaycmd, },
		{"mode", setmodecmd, },
		{"aquisition", setaquisitioncmd, },
		{"clockrange", setcloclrangecmd, },
		{0, 0, },
	};
	if (param.size()) {
		string cmd = shift(param);
		for (int i=0; setfuncvector[i].cmd; i++) {
			if (cmd.compare(setfuncvector[i].cmd) == 0) {
				return setfuncvector[i].func(param);
			}
		}
	} else {
		cout << "set <";
		for (int i=0; setfuncvector[i].cmd; i++) {
			cout << setfuncvector[i].cmd << "|";
		}
		cout << ">" << endl << endl;
	}

	return 1;
}

int setdelaycmd(vector<string>& param)
{
	if (pManager && param.size() >= 2) {
		string target = shift(param);
		int val = parseAsDec(shift(param));
		if (target.compare("global") == 0) {
			cout << "Global: setDelay(" << val << ")..." << (pManager->setDelay(val) ? "OK" : "NG") << endl << endl;
		} else if (target.compare("all") == 0) {
			int st = 0;
			int en = pManager->getInterfaceCount();
			for (int i=st; i<en; i++) {
				SoundInterface* psif = pManager->getInterface(i);
				if (psif) {
					cout << "Interface: " << i << " setDelay(" << val << ")..." << (psif->setDelay(val) ? "OK" : "NG") << endl;
				}
				pManager->releaseInterface(psif);
			}
			cout << endl;
		} else {
			int targetid = parseAsDec(target);
			SoundInterface* psif = pManager->getInterface(targetid);
			if (psif) {
				cout << "Interface: " << targetid << " setDelay(" << val << ")..." << (psif->setDelay(val) ? "OK" : "NG") << endl << endl;
				pManager->releaseInterface(psif);
			} else {
				cout << "Invalid parameter: " << target << endl << endl;
				return -1;
			}
		}
	} else {
		cout << "Missing operand." << endl << endl;
		return -1;
	}
	return 1;
}

int setmodecmd(vector<string>& param)
{
	if (pManager || param.size()) {
		int val = parseAsDec(shift(param));
		pManager->setMode(val);
		cout << "setMode(" << val << ")" << endl << endl;
	} else {
		cout << "Missing operand." << endl << endl;
		return -1;
	}
	return 1;
}

int setaquisitioncmd(vector<string>& param)
{
	if (pManager || param.size()) {
		int val = parseAsDec(shift(param));
		pManager->setAcquisitionMode(val);
		cout << "setAcquisitionMode(" << val << ")" << endl << endl;
	} else {
		cout << "Missing operand." << endl << endl;
		return -1;
	}
	return 1;
}

int setcloclrangecmd(vector<string>& param)
{
	if (pManager || param.size()) {
		int val = parseAsDec(shift(param));
		pManager->setAcquisitionModeClockRenge(val);
		cout << "setAcquisitionModeClockRenge(" << val << ")" << endl << endl;
	} else {
		cout << "Missing operand." << endl << endl;
		return -1;
	}
	return 1;
}
