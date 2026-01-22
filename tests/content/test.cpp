// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>

#include "../../sdk/include/CContentPluginIntf.h"

int main(int argc, char* argv[])
{
    std::wstring dllPath = L"../../plugins/sora2/Release/Sora2.dll";
#ifdef _WIN64
    dllPath = L"../../plugins/sora2/x64/Release/Sora2.dll";
#endif

    HMODULE hDLL = LoadLibraryW(dllPath.c_str());
    if (!hDLL)
    {
        std::cout << "Cannot load DLL" << std::endl;
        return -1;
    }

    LPCREATEPLUGIN fCreatePlugin = (LPCREATEPLUGIN)GetProcAddress(hDLL, "CreatePlugin");
    LPDELETEPLUGIN fDeletePlugin = (LPDELETEPLUGIN)GetProcAddress(hDLL, "DeletePlugin");

    LPCLICKMENUITEM fClick = (LPCLICKMENUITEM)GetProcAddress(hDLL, "ClickMenuItem");

    PluginHandle handle = fCreatePlugin();

    LPSETLANGUAGE fSetLanguage = (LPSETLANGUAGE)GetProcAddress(hDLL, "SetLanguage");

    const wchar_t* lang = L"ru-RU";
    fSetLanguage(handle, lang);

    fClick(handle, 0);

    fDeletePlugin(handle);
    return 0;
}
