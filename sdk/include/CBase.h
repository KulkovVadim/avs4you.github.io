//---------------------------------------------------------------------------

#ifndef CBaseH
#define CBaseH
//---------------------------------------------------------------------------

#ifdef PLUGIN_EXPORTS
# define PLUGIN_API __declspec(dllexport)
#else
# define PLUGIN_API __declspec(dllimport)
#endif

namespace Plugins {
    enum class PluginType {
        Unknown = 0,
        Content,
        ImageEffect,
    };

    enum class ContextType {
		Unknown = 0,
        MediaLibrary,
		Video,
		Audio,
		Text,
		Image
	};
}

typedef void* PluginHandle;
typedef void (*AsyncCallback)(wchar_t*, wchar_t*, int, void*);

#endif
