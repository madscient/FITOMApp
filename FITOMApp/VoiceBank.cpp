#include "stdafx.h"
#include "FITOM.h"
#include "FITOMCfg.h"
#include "SoundDev.h"
#include "ADPCM.h"

template < typename T >
CVoiceBankTemplate<T>::CVoiceBankTemplate() : vbank(new T[128]), bankname(0), filename(0)
{
}

template < typename T >
CVoiceBankTemplate<T>::~CVoiceBankTemplate()
{
	if (filename) {
		delete[] filename;
	}
	if (bankname) {
		delete[] bankname;
	}
	if (vbank) {
		delete[] vbank;
	}
}

template < typename T >
void CVoiceBankTemplate<T>::SetBankName(LPCTSTR bname)
{
	if (bankname) {
		delete[] bankname;
	}
	size_t len = 0;
	StringCchLength(bname, 256, &len);
	bankname = new TCHAR[++len];
	StringCchCopy(bankname, len, bname);
}

template < typename T >
void CVoiceBankTemplate<T>::SetFileName(LPCTSTR fname)
{
	if (filename) {
		delete[] filename;
	}
	size_t len = 0;
	StringCchLength(fname, 256, &len);
	filename = new TCHAR[++len];
	StringCchCopy(filename, len, fname);
}

template < typename T >
void CVoiceBankTemplate<T>::GetVoice(int num, T* note)
{
	if (num < 128 && num >= 0 && vbank != 0) {
		*note = vbank[num];
	}
}

template < typename T >
void CVoiceBankTemplate<T>::SetVoice(int num, const T* note)
{
	if (num < 128 && num >= 0 && vbank != 0) {
		vbank[num] = *note;
	}
}

//---------------------------------------

CFMBank::CFMBank() : CVoiceBankTemplate<FMVOICE>()
{
	FMVOICE buf;
	memset((void*)&buf, 0, sizeof(FMVOICE));
	for (int i = 0; i<128; i++) {
		buf.ID = uint32_t(0xff000000L) | i;
		SetVoice(i, &buf);
	}
}

void CFMBank::Renumber(int grp, int bnk)
{
	if (vbank) {
		for (int i = 0; i < 128; i++) {
			vbank[i].ID = ((grp & 0xff) << 24) | ((bnk & 0xff) << 8) | (i & 0xff);
		}
	}
}

CPcmBank::CPcmBank() : CVoiceBankTemplate<PCMPROG>()
{
	::ZeroMemory(vbank, sizeof(PCMPROG) * 128);
}

CDrumBank::CDrumBank() : CVoiceBankTemplate<DRUMMAP>()
{
	::ZeroMemory(vbank, sizeof(DRUMMAP) * 128);
}
