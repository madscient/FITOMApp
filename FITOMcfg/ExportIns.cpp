#include "stdafx.h"
#include "ExportIns.h"

DEVIDMAP devmap[] = {
	{ DEVICE_OPN, VOICE_GROUP_OPNA, _T("OPN"), _T("YM2203"), },
	{ DEVICE_OPNA, VOICE_GROUP_OPNA, _T("OPNA"), _T("YM2608"), },
	{ DEVICE_OPNB, VOICE_GROUP_OPNA, _T("OPNB"), _T("YM2610"), },
	{ DEVICE_OPNC, VOICE_GROUP_OPNA, _T("OPNC"), _T("YMF264"), },
	{ DEVICE_2610B, VOICE_GROUP_OPNA, _T("OPNBB"), _T("YM2610B"), },
	{ DEVICE_F286, VOICE_GROUP_OPNA, _T("OPNBK"), _T("YMF286K"), },
	{ DEVICE_OPN2, VOICE_GROUP_OPNA, _T("OPN2"), _T("YM2612"), },
	{ DEVICE_OPN2C, VOICE_GROUP_OPNA, _T("OPN2C"), _T("YM3438"), },
	{ DEVICE_OPN2L, VOICE_GROUP_OPNA, _T("OPN2L"), _T("YMF276"), },
	{ DEVICE_OPN3L, VOICE_GROUP_OPNA, _T("OPN3L"), _T("YMF288"), },
	{ DEVICE_OPN3_L3, VOICE_GROUP_OPNA, _T("OPN3_L"), _T("YMF294(OPL)"), },
	{ DEVICE_OPN3_N3, VOICE_GROUP_OPNA, _T("OPN3_N"), _T("YMF294(OPN)"), },
	{ DEVICE_OPL, VOICE_GROUP_OPL2, _T("OPL"), _T("YM3526"), },
	{ DEVICE_OPL2, VOICE_GROUP_OPL2, _T("OPL2"), _T("YM3812"), },
	{ DEVICE_OPL3, VOICE_GROUP_OPL3, _T("OPL3"), _T("YMF262"), },
	{ DEVICE_OPL3_2, VOICE_GROUP_OPL2, _T("OPL3_2"), _T("YMF262(2op)"), },
	{ DEVICE_Y8950, VOICE_GROUP_OPL2, _T("Y8950"), _T("YM3801"), },
	{ DEVICE_OPLL, VOICE_GROUP_OPLL, _T("OPLL"), _T("YM2413"), },
	{ DEVICE_OPLL2, VOICE_GROUP_OPLL, _T("OPLL2"), _T("YM2420"), },
	{ DEVICE_OPLLX, VOICE_GROUP_OPLL, _T("OPLLX"), _T("YM2423"), },
	{ DEVICE_OPLLP, VOICE_GROUP_OPLL, _T("OPLLP"), _T("YMF281"), },
	{ DEVICE_OPM, VOICE_GROUP_OPM, _T("OPM"), _T("YM2151"), },
	{ DEVICE_OPP, VOICE_GROUP_OPM, _T("OPP"), _T("YM2164"), },
	{ DEVICE_OPZ, VOICE_GROUP_OPM, _T("OPZ"), _T("YM2414"), },
	{ DEVICE_OPZ2, VOICE_GROUP_OPM, _T("OPZ2"), _T("YM2424"), },
	{ DEVICE_OPK, VOICE_GROUP_OPL2, _T("OPK"), _T("YM7116"), },
	{ DEVICE_OPK2, VOICE_GROUP_OPL2, _T("OPK2"), _T("YM7129"), },
	{ DEVICE_PSG, VOICE_GROUP_PSG, _T("PSG"), _T("AY-3-8910"), },
	{ DEVICE_SSG, VOICE_GROUP_PSG, _T("SSG"), _T("YM2149"), },
	{ DEVICE_DCSG, VOICE_GROUP_PSG, _T("DCSG"), _T("SN76489"), },
	{ DEVICE_APSG, VOICE_GROUP_PSG, _T("APSG"), _T("AY8930"), },
	{ DEVICE_DSG, VOICE_GROUP_PSG, _T("DSG"), _T("YM2163"), },
	{ DEVICE_SCC, VOICE_GROUP_PSG, _T("SCC"), _T("SCC"), },
	{ DEVICE_SCCP, VOICE_GROUP_PSG, _T("SCC+"), _T("SCC+"), },
	{ DEVICE_SAA, VOICE_GROUP_PSG, _T("SAA"), _T("SAA1099"), },
	{ DEVICE_ADPCM, VOICE_GROUP_PCM, _T("ADPCM"), _T("ADPCM"), },
	{ DEVICE_ADPCMA, VOICE_GROUP_PCM, _T("ADPCM_A"), _T("ADPCM_A"), },
	{ DEVICE_ADPCMB, VOICE_GROUP_PCM, _T("ADPCM_B"), _T("ADPCM_B"), },
	{ DEVICE_NONE, 0, 0, 0, },
};

LPCTSTR bankgroup[] = {
	_T("OPM"), _T("OPNA"), _T("OPL2"), _T("OPL3"), _T("OPLL"), _T("PSG"), _T("ADPCM"), 0, 0,
};
int groupcode[] = {
	VOICE_GROUP_OPM, VOICE_GROUP_OPNA, VOICE_GROUP_OPL2, VOICE_GROUP_OPL3, VOICE_GROUP_OPLL, VOICE_GROUP_PSG, VOICE_GROUP_PCM, VOICE_GROUP_RHYTHM, 0,
};

LPCTSTR fixedsection =
_T("\n.Controller Names\n\n")
_T("[FITOM Controllers]\n")
_T("0=Device Select\n")
_T("1=Modulation\n")
_T("2=Breath Controller\n")
_T("4=Foot Controller\n")
_T("5=Portamento Time\n")
_T("6=Data Entry MSB\n")
_T("7=Volume\n")
_T("10=Pan\n")
_T("11=Expression\n")
_T("32=Bank Select LSB\n")
_T("38=Data Entry LSB\n")
_T("65=Portamento\n")
_T("66=Sostenuto\n")
_T("79=Force Damp\n")
_T("84=Portamento Control\n")
_T("89=Instant Voice Address\n")
_T("90=Instant Voice Data\n")
_T("96=Data Increment\n")
_T("97=Data Decrement\n")
_T("98=NRPN LSB\n")
_T("99=NRPN MSB\n")
_T("100=RPN LSB\n")
_T("101=RPN MSB\n")
_T("120=All Sound Off\n")
_T("121=Reset All Controller\n")
_T("123=All Note Off\n")
_T("124=Omni Mode Off\n")
_T("125=Omni Mode On\n")
_T("126=Mono Mode On\n")
_T("127=Poly Mode On\n")
_T("\n.RPN Names\n\n")
_T("[FITOM RPN]\n")
_T("0=Pitch Bend Sensitivity\n")
_T("1=Master Fine Tuning\n")
_T("\n.NRPN Names\n\n")
_T("[FITOM NRPN]\n")
_T("0=PM LFO Rate\n")
_T("1=PM LFO Waveform\n")
_T("2=AM LFO Rate\n")
_T("3=AM LFO Waveform\n")
_T("4097=Physical Channel Assign\n")
_T("6145=Direct Register Address\n")
_T("6146=Direct Register Data\n")
_T("\n\n");

void WritePcmPatchNameBank(CStdioFile& dst, TCHAR* rbnkname, LPCTSTR groupname, LPCTSTR bnkfile)
{
	TCHAR bankname[80];
	::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), bnkfile);
	CString str;
	str.Format(_T("%s %s"), groupname, bankname);
	lstrcpy(rbnkname, str);
	str = _T("[") + str + _T("]\n");
	dst.WriteString(str);
	for (int i = 0; i < 128; i++) {
		CString secname;
		TCHAR param[MAX_PATH];
		TCHAR wavfile[MAX_PATH];
		secname.Format(_T("Prog%i"), i);
		::GetPrivateProfileString(secname, _T("Name"), secname, param, _countof(param), bnkfile);
		sscanf_s(param, _T("%[^,]"), wavfile, _countof(wavfile));
		if (::PathFileExists(wavfile)) {
			::PathStripPath(wavfile);
			::PathRemoveExtension(wavfile);
			str.Format(_T("%i=%s\n"), i, wavfile);
			dst.WriteString(str);
		}
	}
}

void WritePatchNameBank(CStdioFile& dst, TCHAR* rbnkname, LPCTSTR groupname, LPCTSTR bnkfile)
{
	TCHAR bankname[80];
	::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), bnkfile);
	CString str;
	str.Format(_T("%s %s"), groupname, bankname);
	lstrcpy(rbnkname, str);
	str = _T("[") + str + _T("]\n");
	dst.WriteString(str);
	for (int i = 0; i < 128; i++) {
		CString secname;
		TCHAR progname[80];
		secname.Format(_T("Prog%i"), i);
		::GetPrivateProfileString(secname, _T("Name"), secname, progname, _countof(progname), bnkfile);
		str.Format(_T("%i=%s\n"), i, progname);
		dst.WriteString(str);
	}
}

void WriteNoteNameBank(CStdioFile& dst, TCHAR* rbnkname, LPCTSTR bnkfile)
{
	TCHAR bankname[80];
	::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), bnkfile);
	lstrcpy(rbnkname, bankname);
	CString str;
	str.Format(_T("[%s]\n"), bankname);
	dst.WriteString(str);
	for (int i = 0; i < 128; i++) {
		CString secname;
		TCHAR param[256];
		secname.Format(_T("Note%i"), i);
		::GetPrivateProfileString(_T("Bank"), secname, _T("**NONE**"), param, _countof(param), bnkfile);
		if (lstrcmp(param, _T("**NONE**"))) {
			TCHAR notename[80];
			sscanf_s(param, _T("%[^,]"), notename, _countof(notename));
			str.Format(_T("%i=%s\n"), i, notename);
			dst.WriteString(str);
		}
	}
}

void WritePatchNameSection(CStdioFile& dst, DevMapArray& devuse, BankNamesArray& arry)
{
	dst.WriteString(_T(".Patch Names\n"));
	for (int i = 0; i < devuse.size(); i++) {
		LPCTSTR grpname = 0;
		for (int l = 0; bankgroup[l] != 0; l++) {
			if (devuse[i].voicegroup == groupcode[l]) {
				grpname = bankgroup[l];
			}
		}
		for (int j = 0; j < 16; j++) {
			TCHAR bnkfile[MAX_PATH];
			CString keyname;
			keyname.Format(_T("bank%i"), j);
			::GetPrivateProfileString(grpname, keyname, _T("**NONE**"), bnkfile, _countof(bnkfile), _T(".\\FITOM.ini"));
			if (lstrcmp(bnkfile, _T("**NONE**")) != 0 && PathFileExists(bnkfile)) {
				BankNames bnk;
				bnk.group = devuse[i].devid;
				bnk.bank = j;
				WritePatchNameBank(dst, bnk.name, devuse[i].chipname, bnkfile);
				arry.push_back(bnk);
			}
		}
	}
	int count = ::GetPrivateProfileInt(_T("ADPCM"), _T("count"), 0, _T(".\\FITOM.ini"));
	for (int i = 0; i < count; i++) {
		TCHAR param[MAX_PATH];
		CString keyname;
		keyname.Format(_T("device%i"), i);
		::GetPrivateProfileString(_T("ADPCM"), keyname, _T("**NONE**"), param, _countof(param), _T(".\\FITOM.ini"));
		TCHAR dev[40], dum[40], bnkfile[MAX_PATH];
		sscanf_s(param, _T("%[^,],%[^,],%[^\n,]"), dev, _countof(dev), dum, _countof(dum), bnkfile, _countof(bnkfile));
		BankNames bnk;
		bnk.group = DEVICE_ADPCM;
		bnk.bank = i;
		WritePcmPatchNameBank(dst, bnk.name, dev, bnkfile);
		arry.push_back(bnk);
	}
	dst.WriteString(_T("\n[FITOM Drumsets]\n"));
	for (int j = 0; j < 16; j++) {
		TCHAR bnkfile[MAX_PATH];
		CString keyname;
		keyname.Format(_T("prog%i"), j);
		::GetPrivateProfileString(_T("Drum"), keyname, _T("**NONE**"), bnkfile, _countof(bnkfile), _T(".\\FITOM.ini"));
		if (lstrcmp(bnkfile, _T("**NONE**")) != 0 && PathFileExists(bnkfile)) {
			TCHAR bankname[80];
			::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), bnkfile);
			CString str;
			str.Format(_T("%i=%s\n"), j, bankname);
			dst.WriteString(str);
			BankNames bnk;
			bnk.group = DEVICE_RHYTHM;
			bnk.bank = j;
			lstrcpy(bnk.name, bankname);
			arry.push_back(bnk);
		}
	}

}

void WriteNoteNameSection(CStdioFile& dst, BankNamesArray& arry)
{
	dst.WriteString(_T(".Note Names\n"));
	for (int j = 0; j < 16; j++) {
		TCHAR bnkfile[MAX_PATH];
		CString keyname;
		keyname.Format(_T("Prog%i"), j);
		::GetPrivateProfileString(_T("Drum"), keyname, _T("**NONE**"), bnkfile, _countof(bnkfile), _T(".\\FITOM.ini"));
		if (lstrcmp(bnkfile, _T("**NONE**")) != 0 && PathFileExists(bnkfile)) {
			TCHAR tmp[80];
			WriteNoteNameBank(dst, tmp, bnkfile);
		}
	}
}

void WriteInstrumentSection(CStdioFile& dst, DevMapArray& devuse, BankNamesArray& arry)
{
	dst.WriteString(
		_T(".Instrument Definitions\n\n")
		_T("[FITOM]\n")
		_T("Control=FITOMControllers\n")
		_T("RPN=FITOM RPN\n")
		_T("NRPN=FITOM NRPN\n"));
	for (int j = 0; j < devuse.size(); j++) {
		for (int k = 0; k < arry.size(); k++) {
			if (arry[k].group == devuse[j].devid) {
				int bank = devuse[j].devid * 128 + arry[k].bank;
				CString keyname;
				keyname.Format(_T("Patch[%i]=%s\n"), bank, arry[k].name);
				dst.WriteString(keyname);
			}
		}
	}
	dst.WriteString(
		_T("Patch[*]=1..128\n")
		_T("\n[FITOM Drumsets]\n")
		_T("Control=FITOMControllers\n")
		_T("RPN=FITOM RPN\n")
		_T("NRPN=FITOM NRPN\n"));
	dst.WriteString(
		_T("Patch[0]=FITOM Drumsets\n")
		_T("Patch[15360]=FITOM Drumsets\n")
		_T("Patch[*]=1..128\n")
		_T("Key[*,*] = 0..127\n"));
	for (int k = 0; k < arry.size(); k++) {
		if (arry[k].group == DEVICE_RHYTHM) {
			CString keyname;
			keyname.Format(_T("Key[0,%i]=%s\n"), arry[k].bank, arry[k].name);
			dst.WriteString(keyname);
			keyname.Format(_T("Key[15360,%i]=%s\n"), arry[k].bank, arry[k].name);
			dst.WriteString(keyname);
		}
	}
	dst.WriteString(_T("Drum[*,*]=1\n"));
}

void AddDevMapArray(DevMapArray& devuse, DEVIDMAP& devadd)
{
	BOOL add = TRUE;
	for (int i = 0; i < devuse.size(); i++) {
		if (devuse[i].devid == devadd.devid) {
			add = FALSE;
		}
	}
	if (add) {
		devuse.push_back(devadd);
	}
}

void GetDevUse(DevMapArray& devuse)
{
	int count = ::GetPrivateProfileInt(_T("Device"), _T("count"), 0, _T(".\\FITOM.ini"));
	for (int i = 0; devmap[i].devid != DEVICE_NONE; i++) {
		for (int k = 0; k < count; k++) {
			TCHAR devparam[80];
			CString devkey;
			devkey.Format(_T("device%i"), k);
			::GetPrivateProfileString(_T("Device"), devkey, _T("**NONE**"), devparam, _countof(devparam), _T(".\\FITOM.ini"));
			TCHAR devname[80];
			sscanf_s(devparam, _T("%[^:,]"), devname, _countof(devname));
			if (lstrcmp(devname, devmap[i].chipname) == 0 || lstrcmp(devname, devmap[i].chipcode) == 0) {
				AddDevMapArray(devuse, devmap[i]);
				if (devmap[i].devid == DEVICE_OPN || devmap[i].devid == DEVICE_OPNA ||
					devmap[i].devid == DEVICE_OPNB || devmap[i].devid == DEVICE_2610B ||
					devmap[i].devid == DEVICE_F286 || devmap[i].devid == DEVICE_OPN3L) {
					DEVIDMAP ssgdev = { DEVICE_SSG, VOICE_GROUP_PSG, _T("SSG"), _T("YM2149"), };
					AddDevMapArray(devuse, ssgdev);
				}
				if (devmap[i].devid == DEVICE_OPL3) {
					DEVIDMAP opl2dev = { DEVICE_OPL3_2, VOICE_GROUP_OPL2, _T("OPL3_2"), _T("YMF262(2op)"), };
					AddDevMapArray(devuse, opl2dev);
				}
			}
		}
	}
}

void ExportInsFile()
{
	CStdioFile file(_T(".\\FITOM.ins"), CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
	CString str;
	file.WriteString(_T("// FITOM Instrument definition\n"));
	file.WriteString(CTime::GetCurrentTime().Format(_T("// %Y/%m/%d %H:%M:%S\n\n")));
	DevMapArray devuse;
	GetDevUse(devuse);
	BankNamesArray arry;
	WritePatchNameSection(file, devuse, arry);
	WriteNoteNameSection(file, arry);
	file.WriteString(fixedsection);
	WriteInstrumentSection(file, devuse, arry);
	file.Close();
}
