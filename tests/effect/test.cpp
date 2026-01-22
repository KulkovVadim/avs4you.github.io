#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <string>

#include "../../sdk/include/CEffectPluginIntf.h"

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;
    UINT  size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return false;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
    if (!pImageCodecInfo)
        return false;

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return true;
        }
    }

    free(pImageCodecInfo);
    return false;
}

bool makeIteration(const std::wstring& inputPath, const double& completness, LPAPPLYEFFECT apply, void** effectData)
{
    Bitmap* bmp = new Bitmap(inputPath.c_str());
    if (!bmp || bmp->GetLastStatus() != Ok)
    {
        std::cout << "Failed to load image" << std::endl;
        return false;
    }

    int w = bmp->GetWidth();
    int h = bmp->GetHeight();

    BitmapData bmpData;
    Rect rect(0, 0, w, h);

    bmp->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite,
        PixelFormat32bppARGB, &bmpData);

    BYTE* pixels = (BYTE*)bmpData.Scan0;

    HRESULT hr = apply(pixels, w, h, completness, 0, NULL, effectData);

    bmp->UnlockBits(&bmpData);

    if (FAILED(hr))
    {
        delete bmp;
        return false;
    }

    CLSID encoderClsid;
    if (!GetEncoderClsid(L"image/png", &encoderClsid))
    {
        std::wcout << L"PNG encoder not found" << std::endl;
        return false;
    }

    std::wstring output = inputPath + L"_" + std::to_wstring(completness) + L".png";
    bmp->Save(output.c_str(), &encoderClsid, nullptr);

    delete bmp;
    return true;
}

int main(int argc, char* argv[])
{
    std::wstring inputPath = L"PATH_TO_IMAGE";

    std::wstring dllPath = L"../../plugins/effect-vhs/Release/vhs.dll";
#ifdef _WIN64
    dllPath = L"../../plugins/effect-vhs/x64/Release/vhs.dll";
#endif

    HMODULE hDLL = LoadLibraryW(dllPath.c_str());
    if (!hDLL)
    {
        std::cout << "Cannot load DLL" << std::endl;
        return -1;
    }

    LPAPPLYEFFECT ApplyEffect = (LPAPPLYEFFECT)GetProcAddress(hDLL, "ApplyEffect");

    if (!ApplyEffect)
    {
        std::cout << "ApplyEffect not found in DLL" << std::endl;
        return -1;
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    void* effectData = NULL;
    makeIteration(inputPath, 0.1, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.2, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.3, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.4, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.5, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.6, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.7, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.8, ApplyEffect, &effectData);
    makeIteration(inputPath, 0.9, ApplyEffect, &effectData);
    makeIteration(inputPath, 1.0, ApplyEffect, &effectData);

    if (effectData)
    {
        LPRELEASEEFFECTDATA ReleaseData = (LPRELEASEEFFECTDATA)GetProcAddress(hDLL, "ReleaseEffectData");
        if (ReleaseData)
            ReleaseData(effectData);
    }
        
    FreeLibrary(hDLL);
    GdiplusShutdown(gdiplusToken);

    return 0;
}
