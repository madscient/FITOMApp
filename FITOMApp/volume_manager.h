#pragma once

#include <boost/range/irange.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <Windows.h>
#include <MMDeviceAPI.h>
#include <Audiopolicy.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

#pragma comment (lib, "volume_manager.lib")

class ComBase {
private:
   static unsigned int counter;
   static bool initialized;
protected:
   class ReleaseCom {
   public:
      void operator()(IUnknown * const ptr) {
         ptr->Release();
      }
   };
   static HRESULT CoCreateInstance(REFCLSID rclsid, REFIID riid, LPVOID FAR* ppv) {
      if (!initialized) {
         ::CoInitialize(nullptr);
         initialized = true;
      }
      return ::CoCreateInstance(rclsid, nullptr, CLSCTX_INPROC_SERVER, riid, ppv);
   }
public:
   ComBase() {
      counter++;
   }
   ~ComBase() {
      counter--;
      if (counter == 0) {
         ::CoUninitialize();
      }
   }
};

template<typename ComInterface>
class Com : public ComBase {
protected:
   std::unique_ptr<ComInterface, ReleaseCom> ptr;
public:
   Com(ComInterface * const raw) {
      ptr = std::unique_ptr<ComInterface, ReleaseCom>(raw);
   }
   ComInterface *operator->() {
      return ptr.get();
   }
};

template<typename ComInterface, typename UUIDOFType = ComInterface>
class ComCCI : public Com<ComInterface> {
private:
   static ComInterface *create() {
      ComInterface *raw;
      const HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), IID_PPV_ARGS(&raw));
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return raw;
   }
public:
   ComCCI() : Com<ComInterface>(create()) {
   }
};

class SimpleAudioVolume : public Com<ISimpleAudioVolume> {
public:
   SimpleAudioVolume(ISimpleAudioVolume * const raw) : Com<ISimpleAudioVolume>(raw) {
   }
   void Set(const float value) {
      const HRESULT hr = ptr->SetMasterVolume(value, nullptr);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
   }
   float Get() {
      float result;
      const HRESULT hr = ptr->GetMasterVolume(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return result;
   }
};

class AudioSessionControl : public Com<IAudioSessionControl2> {
private:
   class HandleCloser {
   public:
      void operator()(void *ptr) {
         ::CloseHandle(reinterpret_cast<HANDLE>(ptr));
      }
   };
public:
   AudioSessionControl(IAudioSessionControl2 * const raw) : Com<IAudioSessionControl2>(raw) {
   }
   DWORD GetProcessId() {
      DWORD result;
      const HRESULT hr = ptr->GetProcessId(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return result;
   }
   boost::filesystem::path GetProcessImageFilePath() {
      const DWORD processId = GetProcessId();
      const HANDLE processHandleRaw = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, processId);
      std::unique_ptr<void, HandleCloser> processHandle(processHandleRaw);
      wchar_t path[MAX_PATH];
      ::GetProcessImageFileNameW(processHandle.get(), path, _countof(path));
      return path;
   }
   std::wstring GetDisplayName() {
      wchar_t *result;
      const HRESULT hr = ptr->GetDisplayName(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return result;
   }
   SimpleAudioVolume GetVolumeInstance() {
      ISimpleAudioVolume *result;
      const HRESULT hr = ptr->QueryInterface(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return SimpleAudioVolume(result);
   }
   void SetVolume(const float value) {
      GetVolumeInstance().Set(value);
   }
   float GetVolume() {
      return GetVolumeInstance().Get();
   }
};

class AudioSessionControlOld : public Com<IAudioSessionControl> {
public:
   AudioSessionControlOld(IAudioSessionControl * const raw) : Com<IAudioSessionControl>(raw) {
   }
   AudioSessionControl Convert() {
      IAudioSessionControl2 *result;
      const HRESULT hr = ptr->QueryInterface(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return AudioSessionControl(result);
   }
};

class AudioSessionEnumerator : public Com<IAudioSessionEnumerator> {
private:
   unsigned int count;
   unsigned int GetCountRaw() {
      int result;
      const HRESULT hr = ptr->GetCount(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return static_cast<unsigned int>(result);
   }
   class Iterator {
   private:
      AudioSessionEnumerator &ptr;
      unsigned int index;
   public:
      Iterator(AudioSessionEnumerator &ptr, const unsigned int index) : ptr(ptr), index(index) {
      }
      AudioSessionControl operator*() {
         return ptr.GetSession(index);
      }
      void operator++() {
         index++;
      }
      bool operator!=(const Iterator &other) const {
         return index != other.index;
      }
      bool operator==(const Iterator &other) const {
         return index == other.index;
      }
   };
public:
   AudioSessionEnumerator(IAudioSessionEnumerator * const raw) : Com<IAudioSessionEnumerator>(raw) {
      count = GetCountRaw();
   }
   AudioSessionControl GetSession(const unsigned int index) {
      IAudioSessionControl *result;
      const HRESULT hr = ptr->GetSession(index, &result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return AudioSessionControlOld(result).Convert();
   }
   unsigned int GetCount() const {
      return count;
   }
   Iterator begin() {
      return Iterator(*this, 0);
   }
   Iterator end() {
      return Iterator(*this, GetCount());
   }
};

class AudioSessionManager : public Com<IAudioSessionManager2> {
public:
   AudioSessionManager(IAudioSessionManager2 * const raw) : Com<IAudioSessionManager2>(raw) {
   }
   AudioSessionEnumerator GetSessionEnumerator() {
      IAudioSessionEnumerator *result;
      const HRESULT hr = ptr->GetSessionEnumerator(&result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return AudioSessionEnumerator(result);
   }
};

class MMDevice : public Com<IMMDevice> {
public:
   MMDevice(IMMDevice * const raw) : Com<IMMDevice>(raw) {
   }
   AudioSessionManager ActivateSessionManager() {
      IAudioSessionManager2 *result;
      const HRESULT hr = ptr->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void **>(&result));
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return AudioSessionManager(result);
   }
};

class MMDeviceEnumerator : public ComCCI<IMMDeviceEnumerator, MMDeviceEnumerator> {
public:
   MMDevice GetDefaultAudioEndpoint(EDataFlow flow, ERole role) {
      IMMDevice *result;
      const HRESULT hr = ptr->GetDefaultAudioEndpoint(flow, role, &result);
      if (FAILED(hr)) {
         ::ExitProcess(1);
      }
      return MMDevice(result);
   }
};
