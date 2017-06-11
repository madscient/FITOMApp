#include "stdafx.h"
#include "commands.h"

int regcmd(vector<string>& param);
int showcmd(vector<string>& param);
int exitcmd(vector<string>& param);
int waitcmd(vector<string>& param);
int callcmd(vector<string>& param);
int cdcmd(vector<string>& param);
int setcmd(vector<string>& param);
int remcmd(vector<string>& param);

const cmdvect cmdfuncvector[] = {
	{"reg", regcmd, },
	{"set", setcmd, },
	{"show", showcmd, },
	{"wait", waitcmd, },
	{"call", callcmd, },
	{"cd", cdcmd, },
	{"exit", exitcmd, },
	{"rem", remcmd, },
	{0, 0, },
};

int cmdintpr(char* cmd)
{
	vector<string> cmdparam = split(string(cmd), ' ');
	if (cmdparam.size()) {
		string cmdname = shift(cmdparam);
		for (int i=0; cmdfuncvector[i].cmd; i++) {
			if (cmdname.compare(cmdfuncvector[i].cmd) == 0) {
				int res = cmdfuncvector[i].func(cmdparam);
				//pManager->sendData();
				return res;
			}
		}
		system(cmd);
	} else {
		return 1;
	}
	return 1;
}

int exitcmd(vector<string>& param)
{
	int res = 0;
	if (param.size()) {
		res = parseAsDec(shift(param));
	}
	return res;
}

int regcmd(vector<string>& param)
{
	if (param.size() > 2) {
		string chipname = shift(param);
		SoundChip* psc = getSoundChipFromString(chipname);
		if (psc) {
			int reg = parseAsHex(shift(param));
			int data = parseAsHex(shift(param));
			if (reg < 0 || reg > 0xffff || data < 0 || data > 255) {
				cout << "Out of range" << endl << endl;
			} else {
				psc->setRegister(DWORD(reg), DWORD(data));
				//cout << " " << chipname << ": " << hex << setw(3) << setfill('0') << DWORD(reg) << ":" << setw(2) << psc->getRegister(DWORD(reg)) << dec << endl << endl;
			}
			//pManager->releaseSoundChip(psc);
		} else {
			cout << "Chip not found: " << chipname << endl << endl;
		}
	} else {
		cout << "Usage: reg <Chip name|Chip location> <register(hex)> <data(hex)>" << endl << endl;
	}
	return 1;
}

int waitcmd(vector<string>& param)
{
	if (param.size()) {
		int waittime = parseAsDec(shift(param));
		HANDLE hTimer = ::CreateEventA(NULL, FALSE, FALSE, "Dummy timer");
		::WaitForSingleObject(hTimer, waittime);
		::CloseHandle(hTimer);
	}
	return 1;
}

int callcmd(vector<string>& param)
{
	if (param.size()) {
		ifstream fin(param.front().c_str(), ios::in);
		if (fin.is_open()) {
			int rescode = 1;
			char* cmdbuf = new char[1024];
			while (rescode > 0 && !fin.eof()) {
				fin.getline(cmdbuf, 1024);
				cout << cmdbuf << endl;
				rescode = cmdintpr(cmdbuf);
			}
			fin.close();
			return rescode;
		} else {
			cout << "Can't open \"" << param.front() << "\"" << endl << endl;
		}
	} else {
		cout << "Usage: call <filename>" << endl << endl;
	}
	return 1;
}

int cdcmd(vector<string>& param)
{
	if (param.size()) {
		::SetCurrentDirectoryA(join(param, " ").c_str());
	} else {
		char buf[1024];
		DWORD res = ::GetCurrentDirectory(1024, buf);
		cout << buf << endl << endl;
	}
	return 1;
}

int remcmd(vector<string>& param)
{
	return 1;
}

int printSoundChipInfo(SCCI_SOUND_CHIP_INFO* psci)
{
	if (psci) {
		cout << psci->dBusID << ": " << psci->cSoundChipName << "(" << psci->iSoundChip << ") : " << psci->dClock << endl;
	}
	return 0;
}
