#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#if defined(__unix__) || defined(__unix)
#include <dlfcn.h>
#define INFERNUX_API extern "C"
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define INFERNUX_API extern "C" __declspec(dllexport)
#define dlsym GetProcAddress
#endif

#include "InfLog.h"

namespace infutils
{

class InfExtLoad
{
  public:
    bool LoadDLL(const std::string &dllPath, const std::string &dllName)
    {
#if defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
        INFLOG_DEBUG("Loading DLL in linux: ", dllPath.c_str());
        void *hLib = dlopen(dllPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
#else
        INFLOG_DEBUG("Loading DLL in windows: ", dllPath.c_str());
        HMODULE hLib = LoadLibraryA(dllPath.c_str());
#endif
        if (!hLib) {
            #if defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
            INFLOG_FATAL("Cannot to load DLL: ", dllName.c_str(), " Error: ", dlerror());
            #else
            INFLOG_ERROR("Failed to load ", dllName.c_str(), ". Reason: ", GetLastErrorAsString().c_str());
            #endif
            return false;
        }
        m_dlls[dllName] = hLib;
        INFLOG_DEBUG("Succeed to load ", dllName.c_str());
        return true;
    }

    template <typename T>
    T LoadFunc(const std::string &dllName, const std::string &funcName)
    {
        auto it = m_dlls.find(dllName);
        if (it == m_dlls.end())
            INFLOG_FATAL("DLL \"%s\" not loaded", dllName.c_str());

        auto sym = reinterpret_cast<T>(dlsym(it->second, funcName.c_str()));
        if (!sym)
            INFLOG_FATAL("Cannot to load symbol: ", funcName.c_str(), " from ",
                         dllName.c_str());

        INFLOG_DEBUG("Succeed to load symbol: ", funcName.c_str(), " from ",
                     dllName.c_str());
        return sym;
    }

    void UnloadAll()
    {
        for (auto &[name, handle] : m_dlls) {
#ifdef _WIN32
            FreeLibrary(handle);
#else
            dlclose(handle);
#endif
        }
        m_dlls.clear();
    }

    ~InfExtLoad() { UnloadAll(); }

  private:
#ifdef _WIN32
    std::unordered_map<std::string, HMODULE> m_dlls;
    
    std::string GetLastErrorAsString() {
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
            return "No error message"; // 无错误

        LPSTR messageBuffer = nullptr;

        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }
#else
    std::unordered_map<std::string, void *> m_dlls;
#endif
};

} // namespace infutils
