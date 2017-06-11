// scsh.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "commands.h"

SoundInterfaceManager *pManager;

struct scciInterface {
	SoundInterface* psi;
	vector<SoundChip*> scs;
};
vector<scciInterface*> sis;

int _tmain(int argc, _TCHAR* argv[])
{
	int rescode = 1;
	HMODULE	hScci = NULL;

	try {
		// scci.dllの読込み
		if (!(hScci = ::LoadLibrary((LPCSTR)"scci.dll"))) {
			throw exception("Failed to load scci.dll");
		}
		cout << "LoadLibrary...OK" << endl;

		// サウンドインターフェースマネージャー取得用関数アドレス取得
		SCCIFUNC getSoundInterfaceManager  = (SCCIFUNC)(::GetProcAddress(hScci,"getSoundInterfaceManager"));
		if(getSoundInterfaceManager == NULL){
			throw exception("Failed to get interface");
		}
		cout << "GetProcAddress...OK" << endl;

		// サウンドインターフェースマネージャー取得
		if (!(pManager = getSoundInterfaceManager())) {
			throw exception("Failed to get Manager");
		}
		cout << "getSoundInterfaceManager...OK" << endl;
	} catch (exception e) {
		cerr << e.what() << endl;
		return GetLastError();
	}

	// サウンドインターフェースマネージャーインスタンス初期化
	pManager->initializeInstance();

	// リセットを行う
	pManager->init();
	pManager->reset();
	pManager->setLevelDisp(TRUE);

	//pManager->setDelay(0);
	sis.clear();
	int ifcount = pManager->getInterfaceCount();
	cout << "Found " << ifcount << " interface(s) ready." << endl << endl;
	for (int i=0; i<ifcount; i++) {
		scciInterface* psi = new scciInterface();
		psi->psi = pManager->getInterface(i);
		psi->scs.clear();
		for (int j=0; j<psi->psi->getSoundChipCount(); j++) {
			psi->scs.push_back(psi->psi->getSoundChip(j));
		}
		sis.push_back(psi);
	}

	//ここからコマンドインタプリタ
	char* cmdbuf = new char[1024];
	if (argc > 1) {
		lstrcpy(cmdbuf, argv[1]);
		for (int i = 0; i < (argc - 2); i++) {
			lstrcat(cmdbuf, " ");
			lstrcat(cmdbuf, argv[i + 2]);
		}
		cmdintpr(cmdbuf);
		rescode = 0;
	}
	while (rescode != 0) {
		cout << "scsh>";
		cin.getline(cmdbuf, 1024);
		rescode = cmdintpr(cmdbuf);
	}

	pManager->releaseAllSoundChip();
	pManager->releaseAllInterface();
	pManager->releaseInstance();
	FreeLibrary(hScci);
	return 0;
}

vector<string> split(const std::string &str, char sep)
{
    std::vector<std::string> v;
    std::stringstream ss(str);
    std::string buffer;
    while( std::getline(ss, buffer, sep) ) {
        v.push_back(buffer);
    }
    return v;
}

int parseAsDec(string& str)
{
	int ret = -1;
	sscanf_s(str.c_str(), "%i", &ret);
	return ret;
}

int parseAsHex(string& str)
{
	int ret = -1;
	sscanf_s(str.c_str(), "%x", &ret);
	return ret;
}

string shift(vector<string>& strlist)
{
	string ret("");
	if (strlist.size() > 0) {
		ret = strlist.front();
		strlist.erase(strlist.begin());
	}
	return ret;
}

string pop(vector<string>& strlist)
{
	string ret("");
	if (strlist.size() > 0) {
		ret = strlist.back();
		strlist.erase(strlist.end());
	}
	return ret;
}

string join(vector<string>& strlist, const char* delim)
{
	string ret = "";
	for (vector<string>::iterator iter = strlist.begin(); iter != strlist.end(); iter++) {
		ret += *iter + delim;
	}
	return ret;
}

SC_CHIP_TYPE getChipTypeFromName(string& name)
{
	SC_CHIP_TYPE ret = SC_TYPE_NONE;
	if (name.compare("OPN3") == 0 || name.compare("OPN3L") == 0 || name.compare("YMF288") == 0) {
		ret = SC_TYPE_YMF288;
	}
	if (name.compare("OPNA") == 0 || name.compare("YM2608") == 0 || name.compare("YM2608B") == 0) {
		ret = SC_TYPE_YM2608;
	}
	if (name.compare("OPM") == 0 || name.compare("YM2151") == 0) {
		ret = SC_TYPE_YM2151;
	}
	if (name.compare("OPNB") == 0 || name.compare("YM2610") == 0) {
		ret = SC_TYPE_YM2610;
	}
	if (name.compare("OPN") == 0 || name.compare("YM2203") == 0 || name.compare("YM2203C") == 0) {
		ret = SC_TYPE_YM2203;
	}
	if (name.compare("OPN2") == 0 || name.compare("YM2612") == 0) {
		ret = SC_TYPE_YM2203;
	}
	if (name.compare("PSG") == 0 || name.compare("AY-3-8910") == 0 || name.compare("AY8910") == 0 || name.compare("AY38910") == 0) {
		ret = SC_TYPE_AY8910;
	}
	if (name.compare("DCSG") == 0 || name.compare("SN76489") == 0) {
		ret = SC_TYPE_SN76489;
	}
	if (name.compare("OPL2") == 0 || name.compare("YM3812") == 0) {
		ret = SC_TYPE_YM3812;
	}
	if (name.compare("OPL3") == 0 || name.compare("YMF262") == 0) {
		ret = SC_TYPE_YMF262;
	}
	if (name.compare("OPLL") == 0 || name.compare("YM2413") == 0) {
		ret = SC_TYPE_YM2413;
	}
	if (name.compare("OPL") == 0 || name.compare("YM3526") == 0) {
		ret = SC_TYPE_YM3526;
	}
	if (name.compare("SCC") == 0 || name.compare("2212P003") == 0) {
		ret = SC_TYPE_SCC;
	}
	if (name.compare("SCCS") == 0 || name.compare("SCC-I") == 0 || name.compare("SCC+") == 0 || name.compare("2312P001") == 0) {
		ret = SC_TYPE_SCCS;
	}
	if (name.compare("Y8950") == 0 || name.compare("YM3801") == 0 || name.compare("MSX-AUDIO") == 0) {
		ret = SC_TYPE_Y8950;
	}
	if (name.compare("OPP") == 0 || name.compare("YM2164") == 0) {
		ret = SC_TYPE_YM2164;
	}
	if (name.compare("OPZ") == 0 || name.compare("YM2414") == 0) {
		ret = SC_TYPE_YM2414;
	}
	if (name.compare("EPSG") == 0 || name.compare("AY8930") == 0) {
		ret = SC_TYPE_AY8930;
	}
	if (name.compare("SSG") == 0 || name.compare("YM2149") == 0) {
		ret = SC_TYPE_YM2149;
	}
	if (name.compare("SSGLP") == 0 || name.compare("YMZ294") == 0) {
		ret = SC_TYPE_YMZ294;
	}
	if (name.compare("DCSG2") == 0 || name.compare("SN76496") == 0) {
		ret = SC_TYPE_SN76496;
	}
	if (name.compare("OPLL2") == 0 || name.compare("YM2420") == 0) {
		ret = SC_TYPE_YM2420;
	}
	if (name.compare("OPLLP") == 0 || name.compare("YMF281") == 0) {
		ret = SC_TYPE_YMF281;
	}
	if (name.compare("OPLLX") == 0 || name.compare("YM2423") == 0 || name.compare("MS1823") == 0) {
		ret = SC_TYPE_YM2423;
	}
	if (name.compare("OPN2L") == 0 || name.compare("YMF276") == 0) {
		ret = SC_TYPE_YMF276;
	}
	if (name.compare("OPNBB") == 0 || name.compare("YM2610B") == 0) {
		ret = SC_TYPE_YM2610B;
	}
	if (name.compare("OPNBC") == 0 || name.compare("YMF286") == 0) {
		ret = SC_TYPE_YMF286;
	}
	if (name.compare("YM2602") == 0 || name.compare("315-5124") == 0) {
		ret = SC_TYPE_YM2602;
	}
	if (name.compare("UM3567") == 0) {
		ret = SC_TYPE_UM3567;
	}
	if (name.compare("OPL4") == 0 || name.compare("YMF274") == 0) {
		ret = SC_TYPE_YMF274;
	}
	if (name.compare("OPQ") == 0 || name.compare("YM3806") == 0) {
		ret = SC_TYPE_YM3806;
	}
	if (name.compare("DSG") == 0 || name.compare("YM2163") == 0) {
		ret = SC_TYPE_YM2163;
	}
	if (name.compare("OPK2") == 0 || name.compare("YM7129") == 0) {
		ret = SC_TYPE_YM7129;
	}
	if (name.compare("PCM8") == 0 || name.compare("YMZ280") == 0) {
		ret = SC_TYPE_YMZ280;
	}
	if (name.compare("SSGS") == 0 || name.compare("YMZ705") == 0) {
		ret = SC_TYPE_YMZ705;
	}
	if (name.compare("FMS") == 0 || name.compare("YMZ735") == 0) {
		ret = SC_TYPE_YMZ735;
	}
	return ret;
}

SoundChip* getSoundChipFromString(string& name)
{
	if (pManager) {
		if (name.find(':') != string::npos) {
			int ifid = -1;
			int devid = -1;
			sscanf_s(name.c_str(), "%i:%i", &ifid, &devid);
			//SoundInterface* psi = pManager->getInterface(ifid);
			return getSoundChipFromId(ifid, devid);
		} else {
			SC_CHIP_TYPE type = getChipTypeFromName(name);
			for (int i=0; i<sis.size(); i++) {
				for (int j=0; j<sis[i]->scs.size(); j++) {
					if (sis[i]->scs[j]->getSoundChipType() == type) {
						return sis[i]->scs[j];
					}
				}
			}
		}
	}
	return 0;
}

SoundInterface* getSoundInterface(int ifid)
{
	if (ifid >= 0 && ifid < sis.size()) {
		return sis[ifid]->psi;
	}
	return 0;
}

SoundChip* getSoundChipFromId(int ifid, int devid)
{
	if (ifid >= 0 && ifid < sis.size() && devid < sis[ifid]->scs.size() && devid >= 0) {
		return sis[ifid]->scs[devid];
	}
	return 0;
}