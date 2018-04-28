#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

namespace SCCISharp {

	using namespace System;

	SCCISharp::InterfaceInfo::InterfaceInfo(String^ name, Int32 chipCount)
	{
		if (name == nullptr)
			throw gcnew ArgumentNullException("name");

		if (chipCount < 0)
			throw gcnew ArgumentOutOfRangeException("chipCount");

		this->name = name;
		this->chipCount = chipCount;
	}

	String^ SCCISharp::InterfaceInfo::Name::get()
	{
		return this->name;
	}

	Int32 SCCISharp::InterfaceInfo::SoundChipCount::get()
	{
		return this->chipCount;
	}
}