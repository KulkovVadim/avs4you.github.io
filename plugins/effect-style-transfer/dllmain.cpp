// dllmain.cpp : Defines the entry point for the DLL application.
#define PLUGIN_EXPORTS
//#define __cplusplus
#include "./effect.h"
#include <algorithm>
#include <filesystem>
#include "StyleTransferRuntime.h"

std::vector<std::wstring> model_list = {
    // onnx model zoo
    //L"\\models\\rain_princess_dynamic.onnx",
    //L"\\models\\udnie_dynamic.onnx",
    L"\\models\\paints_drawing.onnx",
    L"\\models\\pencil.onnx",
    L"\\models\\scream.onnx",
    // ebylmz
    L"\\models\\candy_cw2.0_sw900000.0_tw2.0.onnx",
    L"\\models\\crystal_grove_cw2.0_sw900000.0_tw2.0.onnx",
    L"\\models\\la_muse_cw2.0_sw900000.0_tw2.0.onnx",
    L"\\models\\mosaic_cw2.0_sw400000.0_tw2.0.onnx",
    L"\\models\\starry_night_cw2.0_sw400000.0_tw2.0.onnx"
};

std::wstring GetModelPath(const int nID)
{
    return Utils::GetDllDirectoryLongPath() + model_list.at(nID);
}

bool InitRuntime(void** p, const int nID)
{
    auto* rt = new StyleTransferRuntime;
    try
    {
        rt->Init(GetModelPath(nID), false, 0);
    }
    catch (...)
    {
        delete rt;
        return false;
    }
    *p = rt;
    return true;
}

extern "C" {
    PLUGIN_API void __stdcall ReleasePluginString(wchar_t* ptr)
    {
       release_export_ptr(ptr);
    }

    PLUGIN_API void __stdcall ReleaseEffectData(void* data)
    {
        auto* v = static_cast<StyleTransferRuntime*>(data);
        delete v;
    }

    PLUGIN_API Plugins::PluginType __stdcall PluginType()
    {
        return Plugins::PluginType::ImageEffect;
    }

    PLUGIN_API wchar_t* __stdcall PluginId()
    {
        return export_str(L"EffectStyleTransfer.plugin");
    }

    PLUGIN_API wchar_t* __stdcall PluginName()
    {
        return export_str(L"StyleTransfer");
    }

    PLUGIN_API wchar_t* __stdcall PluginVersion()
    {
        return export_str(L"1.0.1");
    }

    PLUGIN_API wchar_t* __stdcall PluginIcon()
    {
        return export_str(L"");
    }

    PLUGIN_API bool __stdcall IsApplicationSupported(int id)
    {
        switch (id)
        {
        case AVS_VIDEO_CONVERTER:
        case AVS_VIDEO_EDITOR:
        case AVS_IMAGE_CONVERTER:
            return true;
        default:break;
        }
        return false;
    }

    PLUGIN_API int __stdcall GetEffectsCount()
    {
        return model_list.size();
    }

    PLUGIN_API wchar_t* __stdcall GetEffectName(int index)
    {
        std::wstring name;
        switch (index) {
        //case 2:   return export_str(L"rain princess");
        //case 3:   return export_str(L"udnie");
        case 0:   return export_str(L"paints drawing");
        case 1:   return export_str(L"pencil");
        case 2:   return export_str(L"scream");
        case 3:   return export_str(L"candy-cw2");
        case 4:   return export_str(L"crystal-grove-cw2");
        case 5:   return export_str(L"la-muse-cw2");
        case 6:   return export_str(L"mosaic-cw2");
        case 7:   return export_str(L"starry-night-cw2");
        }

        return export_str(L"StyleTransfer");
    }

    PLUGIN_API wchar_t* __stdcall GetEffectParams(int)
    {
        return NULL;
    }

    PLUGIN_API int __stdcall GetEffectId(int index)
    {
        return index;
    }

    PLUGIN_API HRESULT __stdcall ApplyEffect(BYTE* pPixels, int w, int h, double dCompleteness, int nID, const BSTR* sParams, void** p)
    {
        if (*p == nullptr)
            if (!InitRuntime(p, nID))
                return S_FALSE;

        Utils::DebugPrint("Call ApplyEffect\n");

        if (!pPixels || w <= 0 || h <= 0) {
            Utils::DebugPrint("ApplyEffect: invalid arguments (pPixels=%p, w=%d, h=%d)\n", pPixels, w, h);
            return E_INVALIDARG;
        }

        double mix = std::clamp(dCompleteness, 0.0, 1.0);
        auto* v = static_cast<StyleTransferRuntime*>(*p);
        v->RunBGRA(pPixels, w, h, mix, (nID >= 3));
        return S_OK;
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    //std::wstring dll_path = Utils::GetLoadedModulePath(L"onnxruntime.dll");
    std::wstring path = Utils::GetDllDirectoryLongPath();
    path += +L"\\onnxruntime.dll";
    HMODULE h = LoadLibraryW(path.c_str());

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

