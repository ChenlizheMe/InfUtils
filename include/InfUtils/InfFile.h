#include <filesystem>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>

namespace infutils {
    const char* GetExecutableDir() {
    static std::string path;
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0) throw std::runtime_error("Failed to get executable path");
    path = std::filesystem::path(buffer).parent_path().string();
    return path.c_str();
}

}
#else
#include <unistd.h>
#include <limits.h>
#include <string.h>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace infutils{
    const char* GetExecutableDir() {
    static std::string path;
    char result[PATH_MAX];

    ssize_t len = 0;
#if defined(__linux__)
    len = readlink("/proc/self/exe", result, PATH_MAX);
#elif defined(__APPLE__)
    uint32_t size = sizeof(result);
    if (_NSGetExecutablePath(result, &size) != 0)
        throw std::runtime_error("Buffer too small for executable path");
    len = strlen(result);
#endif

    if (len <= 0) throw std::runtime_error("Failed to get executable path");
    path = std::filesystem::path(result, result + len).parent_path().string();
    return path.c_str();
}
}
#endif
namespace infutils{
    const char* JoinPath(std::initializer_list<const char*> parts) {
    static std::string result;
    std::filesystem::path path;

    for (const auto& part : parts) {
        path /= part;
    }

    result = path.string();
    return result.c_str();
}
}