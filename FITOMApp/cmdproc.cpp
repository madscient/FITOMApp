#include "stdafx.h"
#include "FITOM.h"


CFITOM::cmdvect CFITOM::commandtable[] = {
	{ "SetPresetVoice", &CFITOM::cmdSetVoice, },
{ "GetPresetVoice", &CFITOM::cmdGetVoice, },
{ "GetMidiStatus", &CFITOM::cmdGetMidiStatus, },
{ "GetDeviceStatus", &CFITOM::cmdGetDeviceStatus, },
{ "SendMidi", &CFITOM::cmdSendMidi, },
{ "GetPhysicalDevice", &CFITOM::cmdGetPhysicalDevice, },
{ "GetLogicalDevice", &CFITOM::cmdGetLogicalDevice, },
{ "GetMidiDevice", &CFITOM::cmdGetMidiDevice, },
{ "SetPcmVoice", &CFITOM::cmdSetPcmVoice, },
{ "GetPcmVoice", &CFITOM::cmdGetPcmVoice, },
{ "SetWaveForm", &CFITOM::cmdSetWaveForm, },
{ "GetWaveForm", &CFITOM::cmdGetWaveForm, },
{ "SetDeviceReg", &CFITOM::cmdSetDeviceReg, },
{ "GetDeviceReg", &CFITOM::cmdGetDeviceReg, },
};

bool CFITOM::cmdSetVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetMidiStatus(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetDeviceStatus(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdSendMidi(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetPhysicalDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetLogicalDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetMidiDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdSetPcmVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetPcmVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdSetWaveForm(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetWaveForm(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdSetDeviceReg(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

bool CFITOM::cmdGetDeviceReg(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}



bool CFITOM::CmdProc(TCHAR* cmd, TCHAR* result, int reslen)
{
	bool ret = false;
	TCHAR cmdstr[128];
	TCHAR param[65536];
	sscanf_s(cmd, _T("%s %[^\n]"), cmdstr, _countof(cmdstr), param, _countof(param));
	size_t len = strnlen(cmdstr, _countof(cmdstr));
	for each (cmdvect var in commandtable)
	{
		if (tcscmpn(var.cmd, cmdstr, len) == 0) {
			return (this->*(var.func))(param, result, reslen);
		}
	}
	return ret;
}

