#pragma once

typedef int (*cmdfunc)(vector<string>&);
typedef struct {
	const char* cmd;
	cmdfunc func;
} cmdvect;

int cmdintpr(char* cmd);
int regcmd(vector<string>& param);
int showcmd(vector<string>& param);
int exitcmd(vector<string>& param);
int waitcmd(vector<string>& param);
int callcmd(vector<string>& param);
int shellcmd(vector<string>& param);

int printSoundChipInfo(SCCI_SOUND_CHIP_INFO* psci);
