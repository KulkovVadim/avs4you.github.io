#pragma once
#include <string>
#include <vector>

namespace Utils
{
    inline std::wstring GetDllDirectoryLongPath()
    {
        HMODULE hModule = nullptr;
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetDllDirectoryLongPath),
            &hModule
        );

        DWORD size = 4096;
        std::vector<wchar_t> buffer;

        while (true)
        {
            buffer.resize(size);
            DWORD len = GetModuleFileNameW(hModule, buffer.data(), size);

            if (len == 0)
                return L"";

            if (len < size - 1)
                break;

            size *= 2;
        }

        std::wstring fullPath(buffer.data());
        size_t pos = fullPath.find_last_of(L"\\/");
        return (pos != std::wstring::npos) ? fullPath.substr(0, pos) : L"";
    }

    inline void DebugPrint(const char* fmt, ...)
    {
#ifdef _DEBUG
        char buf[1024];

        va_list args;
        va_start(args, fmt);
        vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
        va_end(args);

        OutputDebugStringA(buf);
#endif // _DEBUG
    }

    inline std::wstring GetLoadedModulePath(const wchar_t* moduleName)
    {
        HMODULE h = GetModuleHandleW(moduleName);
        if (!h) return L"";

        wchar_t path[MAX_PATH];
        DWORD n = GetModuleFileNameW(h, path, MAX_PATH);
        return (n ? std::wstring(path, n) : L"");
    }
}