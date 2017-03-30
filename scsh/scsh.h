#pragma once

extern SoundInterfaceManager *pManager;
vector<string> split(const std::string &str, char sep);
string join(vector<string>& strlist, const char* delim);

int parseAsDec(string& str);
int parseAsHex(string& str);
string shift(vector<string>& strlist);
string pop(vector<string>& strlist);

SC_CHIP_TYPE getChipTypeFromName(string& name);
SoundChip* getSoundChipFromString(string& name);
SoundChip* getSoundChipFromId(int ifid, int devid);
SoundInterface* getSoundInterface(int ifid);