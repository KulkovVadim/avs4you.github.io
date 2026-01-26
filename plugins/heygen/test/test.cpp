// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>

#include "../../../sdk/include/CContentPluginIntf.h"


const wchar_t clb_data[] = L"Custom data";

void onCallback(wchar_t* str1, wchar_t* str2, int num, void* data)
{
    const wchar_t* _clb_data = (const wchar_t*)data;

}

int main(int argc, char* argv[])
{
    std::wstring dllPath = L"../../Build/Win32/Debug/Heygen.dll";
#ifdef _WIN64
    dllPath = L"../../../Build/x64/Debug/Heygen.dll";
#endif

    HMODULE hDLL = LoadLibraryW(dllPath.c_str());
    if (!hDLL) {
        std::cout << "Cannot load DLL" << std::endl;
        return -1;
    }

    LPCREATEPLUGIN fCreatePlugin = (LPCREATEPLUGIN)GetProcAddress(hDLL, "CreatePlugin");
    LPDELETEPLUGIN fDeletePlugin = (LPDELETEPLUGIN)GetProcAddress(hDLL, "DeletePlugin");
    LPPLUGINICON fGetIcon = (LPPLUGINICON)GetProcAddress(hDLL, "PluginIcon");
    LPSETLANGUAGE fSetLanguage = (LPSETLANGUAGE)GetProcAddress(hDLL, "SetLanguage");
    LPSETCALLBACKHANDLER fSetCallback = (LPSETCALLBACKHANDLER)GetProcAddress(hDLL, "SetCallbackHandler");
    LPCLICKMENUITEM fClick = (LPCLICKMENUITEM)GetProcAddress(hDLL, "ClickMenuItem");

    PluginHandle handle = fCreatePlugin();

    const wchar_t* iconPath = fGetIcon(handle);
    const wchar_t* lang = L"ru-RU";
    fSetLanguage(handle, lang);
    fSetCallback(handle, onCallback, (void*)clb_data);
    fClick(handle, 201);

    fDeletePlugin(handle);
    return 0;
}
