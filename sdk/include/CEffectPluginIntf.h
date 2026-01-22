//---------------------------------------------------------------------------
/**
 * @file CEffectPluginIntf.h
 * @brief Effect plugin interface
 * @details Defines the API for plugins that provide various effects
 *          for image or video processing
 */

#ifndef CEffectPluginIntfH
#define CEffectPluginIntfH
//---------------------------------------------------------------------------

#include "CBase.h"
#include <oleauto.h>

#ifdef __cplusplus
extern "C" {
#endif

    // ========================================================================
    // Plugin Information
    // ========================================================================

    /**
     * @brief Returns the plugin type
     * @return Plugin type from the Plugins::PluginType enumeration
     */
    PLUGIN_API Plugins::PluginType __stdcall PluginType();

    /**
     * @brief Function pointer type for PluginType
     */
    typedef Plugins::PluginType (__stdcall *LPPLUGINTYPE)();

    /**
     * @brief Returns the unique plugin identifier
     * @return Pointer to a string containing the plugin ID (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall PluginId();

    /**
     * @brief Function pointer type for PluginId
     */
    typedef wchar_t* (__stdcall *LPPLUGINID)();

    /**
     * @brief Returns the plugin name
     * @return Pointer to a string containing the plugin name (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall PluginName();

    /**
     * @brief Function pointer type for PluginName
     */
    typedef wchar_t* (__stdcall *LPPLUGINNAME)();

    /**
     * @brief Returns the plugin version
     * @return Pointer to a string containing the plugin version (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall PluginVersion();

    /**
     * @brief Function pointer type for PluginVersion
     */
    typedef wchar_t* (__stdcall *LPPLUGINVERSION)();

    /**
     * @brief Returns the path to the plugin icon
     * @return Pointer to a string containing the icon path (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall PluginIcon();

    /**
     * @brief Function pointer type for PluginIcon
     */
    typedef wchar_t* (__stdcall *LPPLUGINICON)();

    /**
     * @brief Checks if an application is supported
     * @param appId Application identifier
     * @return true if the application is supported, false otherwise
     */
    PLUGIN_API bool __stdcall IsApplicationSupported(int appId);

    /**
     * @brief Function pointer type for IsApplicationSupported
     */
    typedef bool (__stdcall *LPISAPPLICATIONSUPPORTED)(int);

    /**
     * @brief Returns extra info about plugins
     * @return json
     */
    PLUGIN_API wchar_t* __stdcall PluginInfo();

    /**
     * @brief Function pointer type for PluginInfo
     */
    typedef wchar_t* (__stdcall *LPPLUGININFO)();

    // ========================================================================
    // Memory Management
    // ========================================================================

    /**
     * @brief Releases memory allocated for a plugin string
     * @param str Pointer to the string to be released
     * @details Must be called for all strings obtained from the plugin
     */
    PLUGIN_API void __stdcall ReleasePluginString(wchar_t* str);

    /**
     * @brief Function pointer type for ReleasePluginString
     */
    typedef void (__stdcall *LPRELEASEPLUGINSSTRING)(wchar_t*);

    /**
     * @brief Releases memory allocated for effect data
     * @param data Pointer to the data to be released
     */

    PLUGIN_API void __stdcall ReleaseEffectData(void* data);
    /**
     * @brief Function pointer type for ReleaseEffectData
     */
    typedef void (__stdcall *LPRELEASEEFFECTDATA)(void*);

    // ========================================================================
    // Effect Operations
    // ========================================================================

    /**
     * @brief Returns the number of available effects
     * @return Number of effects in the plugin
     */
    PLUGIN_API int __stdcall GetEffectsCount();

    /**
     * @brief Function pointer type for GetEffectsCount
     */
    typedef int (__stdcall *LPGETEFFECTCOUNT)();

    /**
     * @brief Returns the name of an effect by index
     * @param index Effect index (from 0 to GetEffectsCount()-1)
     * @return Pointer to a string containing the effect name (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall GetEffectName(int index);

    /**
     * @brief Function pointer type for GetEffectName
     */
    typedef wchar_t* (__stdcall *LPGETEFFECTNAME)(int);

    /**
     * @brief Returns the identificator of an effect by index
     * @param index Effect index (from 0 to GetEffectsCount()-1)
     * @return Integer identificator of effect
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API int __stdcall GetEffectId(int index);

    /**
     * @brief Function pointer type for GetEffectId
     */
    typedef int (__stdcall *LPGETEFFECTID)(int);

    /**
     * @brief Returns the parameters of an effect by index
     * @param index Effect index (from 0 to GetEffectsCount()-1)
     * @return Pointer to a string containing the effect parameters (wide characters)
     * @note Memory must be released using ReleasePluginString()
     */
    PLUGIN_API wchar_t* __stdcall GetEffectParams(int index);

    /**
     * @brief Function pointer type for GetEffectParams
     */
    typedef wchar_t* (__stdcall *LPGETEFFECTPARAMS)(int);

    /**
     * @brief Applies an effect to image data
     * @param data Pointer to image data (byte array)
     * @param width Image width in pixels
     * @param height Image height in pixels
     * @param timestamp Timestamp (for video)
     * @param paramCount Number of effect parameters
     * @param params Array of effect parameters (BSTR strings)
     * @param[out] effectData Pointer for returning additional effect data
     * @return HRESULT success/error code
     * @retval S_OK Effect successfully applied
     */
    HRESULT PLUGIN_API __stdcall ApplyEffect(
        BYTE* data,
        int width,
        int height,
        double timestamp,
        int paramCount,
        const BSTR* params,
        void** effectData
    );

    /**
     * @brief Function pointer type for ApplyEffect
     */
    typedef HRESULT (__stdcall *LPAPPLYEFFECT)(BYTE*, int, int, double, int, const BSTR*, void**);

#ifdef __cplusplus
}
#endif

#endif