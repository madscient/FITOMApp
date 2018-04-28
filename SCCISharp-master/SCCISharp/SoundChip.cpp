#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

namespace SCCISharp {

	using namespace System;

	SCCISharp::SoundChip::SoundChip(SoundInterfaceManager^ manager, void* soundChip)
	{
		if (manager == nullptr)
			throw gcnew ArgumentNullException("manager");

		if (soundChip == NULL)
			throw gcnew InvalidOperationException("Unable to get sound chip.");

		this->manager = manager;
		this->soundChip = soundChip;
		this->isDisposed = false;
	}

	SCCISharp::SoundChip::~SoundChip()
	{
		if (this->isDisposed)
			return;

		this->!SoundChip();
	}

	SCCISharp::SoundChip::!SoundChip()
	{
		if (this->isDisposed)
			return;

		this->isDisposed = true;

		// Dispose unmanaged resources
		this->manager->ReleaseSoundChip(IntPtr(this->soundChip));
	}

	ChipType SCCISharp::SoundChip::Type::get()
	{
		this->CheckDisposed();
		return (ChipType)((::SoundChip*)this->soundChip)->getSoundChipType();
	}

	Int32 SCCISharp::SoundChip::Clock::get()
	{
		this->CheckDisposed();
		return ((::SoundChip*)this->soundChip)->getSoundChipClock();
	}

	bool SCCISharp::SoundChip::IsDisposed::get()
	{
		return this->isDisposed;
	}

	IntPtr SCCISharp::SoundChip::Pointer::get()
	{
		return IntPtr(this->soundChip);
	}

	SoundChipInfo^ SCCISharp::SoundChip::GetInfo()
	{
		this->CheckDisposed();

		SCCI_SOUND_CHIP_INFO* cinfo = ((::SoundChip*)this->soundChip)->getSoundChipInfo();

		size_t compSChip_num = sizeof(cinfo->iCompatibleSoundChip) / sizeof(cinfo->iCompatibleSoundChip[0]);
		size_t compClock_num = sizeof(cinfo->dCompatibleClock) / sizeof(cinfo->dCompatibleClock[0]);

		array<ChipType>^ compatibleChip = gcnew array<ChipType>(compSChip_num);
		array<Int32>^ compatibleClock = gcnew array<Int32>(compClock_num);

		for (size_t i = 0; i < compSChip_num; i++)
			compatibleChip[i] = (ChipType)cinfo->iCompatibleSoundChip[i];

		for (size_t i = 0; i < compClock_num; i++)
			compatibleClock[i] = (Int32)cinfo->dCompatibleClock[i];

		return gcnew SoundChipInfo(
			gcnew String(cinfo->cSoundChipName),
			(ChipType)cinfo->iSoundChip,
			(IReadOnlyList<ChipType>^)compatibleChip,
			cinfo->dClock,
			(IReadOnlyList<Int32>^)compatibleClock,
			!!cinfo->bIsUsed,
			cinfo->dBusID,
			(SoundLocation)cinfo->dSoundLocation);
	}

	bool SCCISharp::SoundChip::SetRegister(Int32 address, Int32 data)
	{
		this->CheckDisposed();

		if (address < 0)
			throw gcnew ArgumentOutOfRangeException("address");

		if (data < 0)
			throw gcnew ArgumentOutOfRangeException("data");

		bool result = !!((::SoundChip*)this->soundChip)->setRegister(address, data);
		this->OnSetRegister(this, gcnew SCCISharp::SetRegisterEventArgs(address, data));

		return result;
	}

	Int32 SCCISharp::SoundChip::GetRegister(Int32 address)
	{
		this->CheckDisposed();

		if (address < 0)
			throw gcnew ArgumentOutOfRangeException("address");

		return ((::SoundChip*)this->soundChip)->getRegister(address);
	}

	bool SCCISharp::SoundChip::Initialize()
	{
		this->CheckDisposed();
		return !!((::SoundChip*)this->soundChip)->init();
	}

	void SCCISharp::SoundChip::CheckDisposed()
	{
		if (this->isDisposed)
			throw gcnew ObjectDisposedException("SoundChip");

		if (this->manager->IsDisposed)
			throw gcnew ObjectDisposedException("SoundInterfaceManager");
	}
}