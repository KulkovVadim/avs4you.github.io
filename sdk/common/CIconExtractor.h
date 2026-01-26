#pragma once
#include <Windows.h>
#include <string>

class CIconExtractor {
public:
    /**
     *  Gets icon from own resources
     */
    static bool ExtractIcon(HMODULE hModule, LPCTSTR iconResourceName, const std::wstring& filePath);
//    static bool ExtractIconFromBinary(const std::wstring& exePath, LPCTSTR iconResourceName, const std::wstring& outputPath);

    static int GetIconCount(HMODULE hModule, LPCTSTR iconResourceName);

private:
    #pragma pack(push, 2)

    struct ICONDIR {
        WORD idReserved;   // reserved (must be 0)
        WORD idType;       // 1 for icon, 2 for cursor
        WORD idCount;      // images count
    };

    struct ICONDIRENTRY {
        BYTE  bWidth;          // (0 means 256)
        BYTE  bHeight;         // (0 means 256)
        BYTE  bColorCount;     // (0 if >= 8bpp)
        BYTE  bReserved;       // reserved (0)
        WORD  wPlanes;         //
        WORD  wBitCount;       //
        DWORD dwBytesInRes;    //
        DWORD dwImageOffset;   //
    };

    struct GRPICONDIR {
        WORD idReserved;
        WORD idType;
        WORD idCount;
    };

    struct GRPICONDIRENTRY {
        BYTE  bWidth;
        BYTE  bHeight;
        BYTE  bColorCount;
        BYTE  bReserved;
        WORD  wPlanes;
        WORD  wBitCount;
        DWORD dwBytesInRes;
        WORD  nID;  // ID instead of offset
    };

    #pragma pack(pop)

    static bool WriteIconToFile(HMODULE hModule, LPCTSTR iconResourceName, const std::wstring& filePath);
};
