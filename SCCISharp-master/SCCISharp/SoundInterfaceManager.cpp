#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

static ::SoundInterfaceManager* manager;
static HMODULE hDll;

static void initialize();

namespace SCCISharp {

	using namespace System;
	using namespace System::Threading;

	SoundInterfaceManager^ SCCISharp::SoundInterfaceManager::GetInstance()
	{
		Monitor::Enter(SoundInterfaceManager::lockObject);

		if (SoundInterfaceManager::instance == nullptr)
			SoundInterfaceManager::instance = gcnew SoundInterfaceManager();

		Monitor::Exit(SoundInterfaceManager::lockObject);

		return SoundInterfaceManager::instance;
	}

	SCCISharp::SoundInterfaceManager::SoundInterfaceManager()
	{
		this->isDisposed = false;
		initialize();
	}

	SCCISharp::SoundInterfaceManager::~SoundInterfaceManager()
	{
		if (this->isDisposed)
			return;

		// Dispose managed resources
		this->CloseLevelDisplay();

		this->!SoundInterfaceManager();
	}

	SCCISharp::SoundInterfaceManager::!SoundInterfaceManager()
	{
		if (this->isDisposed)
			return;

		Monitor::Enter(SoundInterfaceManager::lockObject);

		this->isDisposed = true;

		// Dispose unmanaged resources
		manager->releaseInstance();
		::FreeLibrary(hDll);
		hDll = NULL;

		SoundInterfaceManager::instance = nullptr;

		Monitor::Exit(SoundInterfaceManager::lockObject);
	}

	Int32 SCCISharp::SoundInterfaceManager::Version::get()
	{
		this->CheckDisposed();
		return manager->getVersion();
	}

	Int32 SCCISharp::SoundInterfaceManager::Delay::get()
	{
		this->CheckDisposed();
		return manager->getDelay();
	}

	void SCCISharp::SoundInterfaceManager::Delay::set(Int32 value)
	{
		this->CheckDisposed();

		if (value < 0)
			throw gcnew ArgumentOutOfRangeException("value");

		manager->setDelay(value);
	}

	bool SCCISharp::SoundInterfaceManager::ShownLevelDisplay::get()
	{
		this->CheckDisposed();
		return !!manager->isLevelDisp();
	}

	Int32 SCCISharp::SoundInterfaceManager::InterfaceCount::get()
	{
		this->CheckDisposed();
		return manager->getInterfaceCount();
	}

	bool SCCISharp::SoundInterfaceManager::IsDisposed::get()
	{
		return this->isDisposed;
	}

	void SCCISharp::SoundInterfaceManager::ShowLevelDisplay()
	{
		this->CheckDisposed();

		manager->setLevelDisp(true);
	}

	void SCCISharp::SoundInterfaceManager::CloseLevelDisplay()
	{
		this->CheckDisposed();

		manager->setLevelDisp(false);
	}

	InterfaceInfo^ SCCISharp::SoundInterfaceManager::GetInterfaceInfo(Int32 interfaceNumber)
	{
		this->CheckDisposed();

		if (interfaceNumber < 0 || interfaceNumber >= manager->getInterfaceCount())
			throw gcnew ArgumentOutOfRangeException("interfaceNumber");

		SCCI_INTERFACE_INFO* info = manager->getInterfaceInfo(interfaceNumber);

		String^ name = gcnew String(info->cInterfaceName);
		InterfaceInfo^ res = gcnew InterfaceInfo(name, info->iSoundChipCount);

		return res;
	}

	SoundInterface^ SCCISharp::SoundInterfaceManager::GetInterface(Int32 interfaceNumber)
	{
		this->CheckDisposed();

		if (interfaceNumber < 0 || interfaceNumber >= manager->getInterfaceCount())
			throw gcnew ArgumentOutOfRangeException("interfaceNumber");

		::SoundInterface* pSound = manager->getInterface(interfaceNumber);

		if (pSound == NULL)
			throw gcnew InvalidOperationException("Unable to get interface.");

		return gcnew SoundInterface(this, pSound);
	}

	bool SCCISharp::SoundInterfaceManager::ReleaseInterface(IntPtr soundInterface)
	{
		this->CheckDisposed();

		if (soundInterface == IntPtr::Zero)
			throw gcnew ArgumentOutOfRangeException("interfaceNumber");

		return !!manager->releaseInterface((::SoundInterface*)soundInterface.ToPointer());
	}

	bool SCCISharp::SoundInterfaceManager::ReleaseAllInterfaces()
	{
		this->CheckDisposed();
		return !!manager->releaseAllInterface();
	}

	SoundChip^ SCCISharp::SoundInterfaceManager::GetSoundChip(ChipType soundChipType, Int32 clock)
	{
		this->CheckDisposed();

		if (clock < 0)
			throw gcnew ArgumentOutOfRangeException("clock");

		if (!Enum::IsDefined(ChipType::typeid, soundChipType))
			throw gcnew ArgumentOutOfRangeException("soundChipType");

		::SoundChip* pSoundChip = manager->getSoundChip((Int32)soundChipType, clock);

		if (pSoundChip == NULL)
			throw gcnew InvalidOperationException("Unable to get a specified sound chip.");

		return gcnew SoundChip(this, pSoundChip);
	}

	SoundChip^ SCCISharp::SoundInterfaceManager::GetSoundChip(ChipType soundChipType, double clockMhz)
	{
		clockMhz *= 1000000.0;

		if (clockMhz < 0.0 ||
			double::IsNaN(clockMhz) ||
			double::IsInfinity(clockMhz))
			throw gcnew ArgumentOutOfRangeException("clockMhz");

		return this->GetSoundChip(soundChipType, (Int32)clockMhz);
	}

	bool SCCISharp::SoundInterfaceManager::TryGetSoundChip([System::Runtime::InteropServices::Out] SoundChip^% soundChip, ChipType soundChipType, Int32 clock)
	{
		this->CheckDisposed();

		if (clock < 0)
			throw gcnew ArgumentOutOfRangeException("clock");

		if (!Enum::IsDefined(ChipType::typeid, soundChipType))
			throw gcnew ArgumentOutOfRangeException("soundChipType");

		::SoundChip* pSoundChip = manager->getSoundChip((Int32)soundChipType, clock);

		if (pSoundChip == NULL)
		{
			soundChip = nullptr;
			return false;
		}
		else
		{
			soundChip = gcnew SoundChip(this, pSoundChip);
			return true;
		}
	}

	bool SCCISharp::SoundInterfaceManager::TryGetSoundChip([System::Runtime::InteropServices::Out] SoundChip^% soundChip, ChipType soundChipType, double clockMhz)
	{
		clockMhz *= 1000000.0;

		if (clockMhz < 0.0 ||
			double::IsNaN(clockMhz) ||
			double::IsInfinity(clockMhz))
			throw gcnew ArgumentOutOfRangeException("clockMhz");

		return this->TryGetSoundChip(soundChip, soundChipType, (Int32)clockMhz);
	}

	bool SCCISharp::SoundInterfaceManager::ReleaseSoundChip(IntPtr soundChip)
	{
		this->CheckDisposed();

		if (soundChip == IntPtr::Zero)
			throw gcnew InvalidOperationException();

		return !!manager->releaseSoundChip((::SoundChip*)soundChip.ToPointer());
	}

	bool SCCISharp::SoundInterfaceManager::ReleaseAllSoundChips()
	{
		this->CheckDisposed();
		return !!manager->releaseAllSoundChip();
	}

	bool SCCISharp::SoundInterfaceManager::Reset()
	{
		this->CheckDisposed();
		return !!manager->reset();
	}

	bool SCCISharp::SoundInterfaceManager::Initialize()
	{
		this->CheckDisposed();
		return !!manager->init();
	}

	void SCCISharp::SoundInterfaceManager::SetMode(CommunicationMode mode)
	{
		this->CheckDisposed();

		if (!Enum::IsDefined(CommunicationMode::typeid, mode))
			throw gcnew ArgumentOutOfRangeException("mode");

		manager->setMode((Int32)mode);
	}

	void SCCISharp::SoundInterfaceManager::FlushData()
	{
		this->CheckDisposed();
		manager->sendData();
	}

	void SCCISharp::SoundInterfaceManager::ClearBuffer()
	{
		this->CheckDisposed();
		manager->clearBuff();
	}

	void SCCISharp::SoundInterfaceManager::SetAcquisitionMode(AcquisitionMode mode)
	{
		this->CheckDisposed();

		if (!Enum::IsDefined(AcquisitionMode::typeid, mode))
			throw gcnew ArgumentOutOfRangeException("mode");

		manager->setAcquisitionMode((Int32)mode);
	}

	void SCCISharp::SoundInterfaceManager::SetAcquisitionModeClockRange(Int32 clock)
	{
		this->CheckDisposed();

		if (clock < 0)
			throw gcnew ArgumentOutOfRangeException("clock");

		manager->setAcquisitionModeClockRenge(clock);
	}

	void SCCISharp::SoundInterfaceManager::CheckDisposed()
	{
		if (this->isDisposed)
			throw gcnew ObjectDisposedException("SoundInterfaceManager");
	}
}

using namespace System;

static void initialize()
{
	SCCIFUNC getSoundInterfaceManager;

	if ((hDll = ::LoadLibrary(L"scci.dll")) == NULL)
		throw gcnew DllNotFoundException("scci.dll is not found.");

	if ((getSoundInterfaceManager = (SCCIFUNC)GetProcAddress(hDll, "getSoundInterfaceManager"))
		== NULL)
		throw gcnew MemberAccessException("getSoundInterfaceManager is not found.");

	if ((manager = getSoundInterfaceManager()) == NULL)
		throw gcnew MemberAccessException("getSoundInterfaceManager is not found.");

	manager->initializeInstance();
}
