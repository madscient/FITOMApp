#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

namespace SCCISharp {

	using namespace System;

	SCCISharp::SoundChipInfo::SoundChipInfo(
		String^ name,
		ChipType chipType,
		IReadOnlyList<ChipType>^ compatibleChip,
		Int32 clock,
		IReadOnlyList<Int32>^ compatibleChipClock,
		bool isUsed,
		Int32 slot,
		SCCISharp::SoundLocation soundLocation)
	{
		this->name = name;
		this->chipType = chipType;
		this->compatibleChip = compatibleChip;
		this->clock = clock;
		this->compatibleChipClock = compatibleChipClock;
		this->isUsed = isUsed;
		this->slot = slot;
		this->soundLocation = soundLocation;
	}


	String^ SCCISharp::SoundChipInfo::Name::get()
	{
		return this->name;
	}

	ChipType SCCISharp::SoundChipInfo::Type::get()
	{
		return this->chipType;
	}

	IReadOnlyList<ChipType>^ SCCISharp::SoundChipInfo::CompatibleChipType::get()
	{
		return this->compatibleChip;
	}

	Int32 SCCISharp::SoundChipInfo::Clock::get()
	{
		return this->clock;
	}

	IReadOnlyList<Int32>^ SCCISharp::SoundChipInfo::CompatibleChipClock::get()
	{
		return this->compatibleChipClock;
	}

	bool SCCISharp::SoundChipInfo::IsUsed::get()
	{
		return this->isUsed;
	}

	Int32 SCCISharp::SoundChipInfo::Slot::get()
	{
		return this->slot;
	}

	SoundLocation SCCISharp::SoundChipInfo::SoundLocation::get()
	{
		return this->soundLocation;
	}
}