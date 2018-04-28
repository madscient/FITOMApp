#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

namespace SCCISharp {

	using namespace System;

	SCCISharp::SoundInterface::SoundInterface(SoundInterfaceManager^ manager, void* soundInterface)
	{
		this->manager = manager;
		this->soundInterface = soundInterface;

		this->isDisposed = false;
	}

	SCCISharp::SoundInterface::~SoundInterface()
	{
		if (this->isDisposed)
			return;

		// Dispose managed resources

		this->!SoundInterface();
	}

	SCCISharp::SoundInterface::!SoundInterface()
	{
		if (this->isDisposed)
			return;

		this->isDisposed = true;

		// Dispose unmanaged resources
		this->manager->ReleaseInterface(IntPtr(this->soundInterface));
	}

	bool SCCISharp::SoundInterface::IsSupportingLowLevelApi::get()
	{
		this->CheckDisposed();
		return !!((::SoundInterface*)this->soundInterface)->isSupportLowLevelApi();
	}

	Int32 SCCISharp::SoundInterface::Delay::get()
	{
		this->CheckDisposed();
		return ((::SoundInterface*)this->soundInterface)->getDelay();
	}

	void SCCISharp::SoundInterface::Delay::set(Int32 value)
	{
		this->CheckDisposed();

		if (value < 0)
			throw gcnew ArgumentOutOfRangeException("value");

		((::SoundInterface*)this->soundInterface)->setDelay(value);
	}

	Int32 SCCISharp::SoundInterface::SoundChipCount::get()
	{
		this->CheckDisposed();
		return ((::SoundInterface*)this->soundInterface)->getSoundChipCount();
	}

	bool SCCISharp::SoundInterface::IsDisposed::get()
	{
		return this->isDisposed;
	}

	IntPtr SCCISharp::SoundInterface::Pointer::get()
	{
		return IntPtr(this->soundInterface);
	}

	bool SCCISharp::SoundInterface::SendData(array<Byte>^ data, Int32 offset, Int32 length)
	{
		this->CheckDisposed();

		if (data == nullptr)
			throw gcnew ArgumentNullException("data");

		if (offset < 0 || offset >= data->Length)
			throw gcnew ArgumentOutOfRangeException("offset");

		if (length < 0 || length + offset >= data->Length)
			throw gcnew ArgumentOutOfRangeException("length");

		BYTE* buffer = (BYTE*)malloc(sizeof(BYTE)* length);
		BYTE* bufptr = buffer;

		for (size_t i = offset, l = offset + length; i < l; i++)
		{
			*bufptr = data[i];
			bufptr++;
		}

		bool res = !!((::SoundInterface*)this->soundInterface)->setData(buffer, length);
		free(buffer);
		return res;
	}

	Int32 SCCISharp::SoundInterface::ReceiveData(array<Byte>^ data, Int32 offset, Int32 length)
	{
		this->CheckDisposed();

		if (data == nullptr)
			throw gcnew ArgumentNullException("data");

		if (offset < 0 || offset >= data->Length)
			throw gcnew ArgumentOutOfRangeException("offset");

		if (length < 0 || length + offset >= data->Length)
			throw gcnew ArgumentOutOfRangeException("length");

		BYTE* buffer = (BYTE*)malloc(sizeof(BYTE) * length);
		BYTE* bufptr = buffer;
		Int32 readLength = (Int32)((::SoundInterface*)this->soundInterface)->getData(buffer, length);

		for (size_t i = offset, l = offset + length; i < l; i++)
		{
			data[i] = *bufptr;
			bufptr++;
		}

		free(buffer);

		return readLength;
	}

	bool SCCISharp::SoundInterface::Reset()
	{
		this->CheckDisposed();

		return !!((::SoundInterface*)this->soundInterface)->reset();
	}

	bool SCCISharp::SoundInterface::Initialize()
	{
		this->CheckDisposed();

		return !!((::SoundInterface*)this->soundInterface)->init();
	}

	SoundChip^ SCCISharp::SoundInterface::GetSoundChip(Int32 number)
	{
		this->CheckDisposed();

		if (number < 0 || number >= this->SoundChipCount)
			throw gcnew ArgumentOutOfRangeException("number");

		return gcnew SoundChip(this->manager, ((::SoundInterface*)this->soundInterface)->getSoundChip(number));
	}

	void SCCISharp::SoundInterface::CheckDisposed()
	{
		if (this->isDisposed)
			throw gcnew ObjectDisposedException("SoundInterface");

		if (this->manager->IsDisposed)
			throw gcnew ObjectDisposedException("SoundInterfaceManager");
	}
}