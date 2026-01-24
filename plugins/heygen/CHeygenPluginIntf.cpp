//---------------------------------------------------------------------------

#pragma hdrstop

#include "CHeygenPluginIntf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#ifdef __cplusplus
extern "C" {
#endif
    PLUGIN_API PluginHandle __stdcall CreatePlugin()
    {
        return new CAIPlugin();
    }

    PLUGIN_API void __stdcall DeletePlugin(PluginHandle ptr)
    {
        if (ptr) {
            delete ptr, ptr = nullptr;
        }
    }

	PLUGIN_API Plugins::PluginType __stdcall PluginType()
    {
        return Plugins::PluginType::Content;
    }

    PLUGIN_API wchar_t* __stdcall PluginId()
    {
    	return utils::wstr_to_ptr(L"ai.plugin");
    }

    PLUGIN_API wchar_t* __stdcall PluginName()
    {
    	return utils::wstr_to_ptr(L"AI");
    }

    PLUGIN_API wchar_t* __stdcall PluginVersion()
    {
        return utils::wstr_to_ptr(L"0.1");
    }

    PLUGIN_API wchar_t* __stdcall PluginIcon(PluginHandle h)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                return ptr->Icon();
            }
        }

        return NULL;
    }

    PLUGIN_API bool __stdcall IsApplicationSupported(int id)
    {
		return id == AVS_VIDEO_EDITOR;
    }

	PLUGIN_API void __stdcall ReleasePluginString(wchar_t* ptr)
    {
        delete[] ptr;
    }

	PLUGIN_API void __stdcall SetLanguage(PluginHandle h, wchar_t* l)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                ptr->SetLanguage(l);
            }
        }
    }

	PLUGIN_API wchar_t* __stdcall GetMenuForContext(PluginHandle h, Plugins::ContextType t)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                return ptr->GetMenuForContext(t);
            }
        }

        return NULL;
    }

	PLUGIN_API wchar_t* __stdcall GetPluginMenu(PluginHandle h)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
				return ptr->GetPluginMenu();
			}
		}

		return NULL;
	}

	PLUGIN_API wchar_t* __stdcall GetIconById(PluginHandle h, int id)
	{
		if ( h ) {
			CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
			if ( ptr ) {
                return ptr->GetIconById(id);
            }
        }

        return NULL;
    }

	PLUGIN_API void __stdcall ClickMenuItem(PluginHandle h, int id)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                ptr->ClickMenuItem(id);
            }
        }
    }

    PLUGIN_API void __stdcall SetCallbackHandler(PluginHandle h, void* cbh)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                ptr->SetCallbackHandler(cbh);
            }
        }
    }

    PLUGIN_API void __stdcall CleanTemporaryFiles(PluginHandle h)
    {
        if ( h ) {
            CAIPlugin* ptr = dynamic_cast<CAIPlugin*>(h);
            if ( ptr ) {
                ptr->CleanTemporaryFiles();
            }
        }
    }


#ifdef __cplusplus
}
#endif
