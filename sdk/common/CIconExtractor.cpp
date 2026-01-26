#include "CIconExtractor.h"
#include <vector>
#include <fstream>
#include <iostream>

bool CIconExtractor::ExtractIcon(HMODULE hModule, LPCTSTR iconResourceName, const std::wstring& filePath)
{
    if (!hModule) {
        hModule = GetModuleHandle(NULL);
    }

    return WriteIconToFile(hModule, iconResourceName, filePath);
}

#if 0
bool CIconExtractor::ExtractAndSaveIconFromFile(const std::wstring& exePath, LPCTSTR iconResourceName, const std::wstring& outputPath)
{
    HMODULE hModule = LoadLibraryExW(exePath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);

    if (!hModule) {
        std::wcerr << L"Failed to load module: " << exePath << std::endl;
        std::wcerr << L"Error code: " << GetLastError() << std::endl;
        return false;
    }

    bool result = WriteIconToFile(hModule, iconResourceName, outputPath);

    FreeLibrary(hModule);
    return result;
}
#endif

int CIconExtractor::GetIconCount(HMODULE hModule, LPCTSTR iconResourceName)
{
    if (!hModule) {
        hModule = GetModuleHandle(NULL);
    }
    
    // find icons group (RT_GROUP_ICON)
    HRSRC hRsrc = FindResource(hModule, iconResourceName, RT_GROUP_ICON);
    if (!hRsrc) {
        return 0;
    }
    
    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (!hGlobal) {
        return 0;
    }
    
    GRPICONDIR* pGrpIconDir = (GRPICONDIR*)LockResource(hGlobal);
    if (!pGrpIconDir) {
        return 0;
    }
    
    return pGrpIconDir->idCount;
}

bool CIconExtractor::WriteIconToFile(HMODULE hModule, LPCTSTR iconResourceName, const std::wstring& filePath)
{
    HRSRC hRsrc = FindResource(hModule, iconResourceName, RT_GROUP_ICON);
    if (!hRsrc) {
        std::wcerr << L"Icon resource not found" << std::endl;
        std::wcerr << L"Error code: " << GetLastError() << std::endl;
        return false;
    }

    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (!hGlobal) {
        std::wcerr << L"Failed to load icon resource" << std::endl;
        return false;
    }

    GRPICONDIR* pGrpIconDir = (GRPICONDIR*)LockResource(hGlobal);
    if (!pGrpIconDir) {
        std::wcerr << L"Failed to lock resource" << std::endl;
        return false;
    }

    GRPICONDIRENTRY* pGrpIconEntry = (GRPICONDIRENTRY*)(pGrpIconDir + 1);

    int iconCount = pGrpIconDir->idCount;
    std::wcout << L"Found " << iconCount << L" icon(s) in resource" << std::endl;

    std::vector<std::vector<BYTE>> iconData(iconCount);
    std::vector<DWORD> iconSizes(iconCount);

    for (int i = 0; i < iconCount; i++) {
        HRSRC hIconRsrc = FindResource(
            hModule,
            MAKEINTRESOURCE(pGrpIconEntry[i].nID),
            RT_ICON
        );

        if (!hIconRsrc) {
            std::wcerr << L"Failed to find icon #" << i << std::endl;
            continue;
        }

        DWORD iconSize = SizeofResource(hModule, hIconRsrc);
        HGLOBAL hIconGlobal = LoadResource(hModule, hIconRsrc);

        if (!hIconGlobal) {
            std::wcerr << L"Failed to load icon #" << i << std::endl;
            continue;
        }

        BYTE* pIconData = (BYTE*)LockResource(hIconGlobal);

        // Копируем данные иконки
        iconData[i].resize(iconSize);
        memcpy(iconData[i].data(), pIconData, iconSize);
        iconSizes[i] = iconSize;

//        int width = pGrpIconEntry[i].bWidth == 0 ? 256 : pGrpIconEntry[i].bWidth;
//        int height = pGrpIconEntry[i].bHeight == 0 ? 256 : pGrpIconEntry[i].bHeight;

//        std::wcout << L"  Icon #" << i << L": "
//                   << width << L"x" << height
//                   << L", " << pGrpIconEntry[i].wBitCount << L" bpp"
//                   << L", " << iconSize << L" bytes" << std::endl;
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::wcerr << L"Failed to create output file: " << filePath << std::endl;
        return false;
    }

    ICONDIR iconDir;
    iconDir.idReserved = 0;
    iconDir.idType = 1;  // 1 = icon
    iconDir.idCount = iconCount;

    file.write((char*)&iconDir, sizeof(ICONDIR));

    DWORD currentOffset = sizeof(ICONDIR) + iconCount * sizeof(ICONDIRENTRY);

    std::vector<ICONDIRENTRY> entries(iconCount);

    for (int i = 0; i < iconCount; i++) {
        entries[i].bWidth = pGrpIconEntry[i].bWidth;
        entries[i].bHeight = pGrpIconEntry[i].bHeight;
        entries[i].bColorCount = pGrpIconEntry[i].bColorCount;
        entries[i].bReserved = 0;
        entries[i].wPlanes = pGrpIconEntry[i].wPlanes;
        entries[i].wBitCount = pGrpIconEntry[i].wBitCount;
        entries[i].dwBytesInRes = iconSizes[i];
        entries[i].dwImageOffset = currentOffset;

        file.write((char*)&entries[i], sizeof(ICONDIRENTRY));

        currentOffset += iconSizes[i];
    }

    for (int i = 0; i < iconCount; i++) {
        file.write((char*)iconData[i].data(), iconSizes[i]);
    }

    file.close();

    std::wcout << L"Icon successfully saved to: " << filePath << std::endl;
    return true;
}
