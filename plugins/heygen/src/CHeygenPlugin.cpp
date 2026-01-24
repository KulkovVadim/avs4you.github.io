
#include "CHeygenPlugin.h"
#include "aboutdialog.h"
#include "mainwindow.h"
#include "ISLTranslator.h"
#include "resource.h"
#include <nlohmann/json.hpp>
#include "uiapplication.h"
#include "uistyle.h"
#include "uifilesystem.h"
#include "utils.h"
#include "defines.h"
#include "CIconExtractor.h"
#include "IHeygenPlugin.h"
#include "ActivationContext.h"

#define ID_ACTION_SETTINGS 		001
#define ID_ACTION_ABOUT 		002
#define ID_ACTION_GEN_VIDEO 	201
#define ID_ACTION_GEN_AUDIO 	301

#define USE_DELAY_LOAD
#ifdef USE_DELAY_LOAD
//# include <delayimp.h>
//# include "DelayLoad.h"

// PfnDliHook __pfnDliNotifyHook2 = utils::delayload::delayHook;
#endif


class CHeygenPlugin::CHeygenPluginPrivate {
public:
    int app_id = 0;

    AsyncCallback callback_intf = nullptr;
    void* callback_context = nullptr;
public:
	CHeygenPluginPrivate()
	{
	}

	~CHeygenPluginPrivate()
	{
	}

    auto send_ok_callback(const std::wstring& path)
    {
        if ( callback_intf ) {
			callback_intf(utils::wstr_to_ptr({PLUGIN_ID}), utils::wstr_to_ptr(path), 0, callback_context);
        }
    }

    auto send_error_callback(int num)
    {
        if ( callback_intf ) {
			callback_intf(utils::wstr_to_ptr({PLUGIN_ID}), utils::wstr_to_ptr(L"error"), num, callback_context);
        }
    }
};

CHeygenPlugin::CHeygenPlugin()
{
	m_priv = std::make_unique<CHeygenPluginPrivate>();

    std::wstring base_url = L"https://api.heygen.com/v2";

    ISLTranslator::instance().init(L"en", IDT_TRANSLATION);
}

CHeygenPlugin::~CHeygenPlugin()
{
}

void CHeygenPlugin::ReleasePluginString(WCHAR* ptr) const
{
    utils::release_wstr_ptr(ptr);
}

std::wstring CHeygenPlugin::Id() const
{
	return {PLUGIN_ID};
}

std::wstring CHeygenPlugin::Icon() const
{
    return GetIconById(IDI_MAINICON);
}

Plugins::PluginType CHeygenPlugin::Type() const
{
    return Plugins::PluginType::Content;
}

void CHeygenPlugin::SetLanguage(const wchar_t* l)
{
    ISLTranslator::instance().setLanguage(l);
}

std::wstring CHeygenPlugin::GetMenuForContext(Plugins::ContextType type) const
{
    std::wstring json_menu_items;
    nlohmann::json _json = {};
    if ( type == Plugins::ContextType::Video ) {
//        _json.push_back({{"text","Generate avatar video"},{"icon",103},{"action",103}});
//		return _json.dump().c_str();
    } else
    if ( type == Plugins::ContextType::MediaLibrary ) {
        _json.push_back({{"text","Generate avatar video"},{"icon",103},{"action",ID_ACTION_GEN_VIDEO}});
		json_menu_items = utils::utf8_to_wstr(_json.dump());
    } else
    if ( type == Plugins::ContextType::Text ) {

    }

	return json_menu_items;
}

std::wstring CHeygenPlugin::GetPluginMenu() const
{
    nlohmann::json _json = {};
    _json.push_back({{"text","Generate"},
                        {"items", {
                                {{"text","Avatar video"},{"icon",104},{"action",ID_ACTION_GEN_VIDEO}},
                            }
                        }
    			});

    _json.push_back({{"text","Settings"},{"icon",101},{"action", ID_ACTION_SETTINGS}});
    _json.push_back({{"text","About"}, {"icon",102}, {"action", ID_ACTION_ABOUT}});

    std::wstring json_menu_plugin = utils::utf8_to_wstr(_json.dump());
    return json_menu_plugin;
}

std::wstring CHeygenPlugin::GetIconById(int id) const
{
    if ( id == 0 )
        id = 101;

    auto _icon_resource_to_file = [=](int resid, const std::wstring& path) -> bool {
    	// check directory exist and create it if not
        HMODULE hInst = utils::getCurrentModule();
        std::wstring directory;
		const size_t last_slash_idx = path.rfind('\\');
		if ( std::string::npos != last_slash_idx ) {
        	directory = path.substr(0, last_slash_idx);

            if ( !UIFileSystem::dirExists(directory) ) {
                if ( !UIFileSystem::makePath(directory) )
                	return false;
            }
        }

        return CIconExtractor::ExtractIcon(hInst, MAKEINTRESOURCE(resid), path.c_str());
    };

    auto temp_path = UIFileSystem::tempPath();
    auto file_name = L"icon_" + std::to_wstring(id) + L".ico";
    auto icon_path = temp_path + L"/" + Id() + L"/icons" + L"/" + file_name;

    if ( UIFileSystem::fileExists(icon_path) )
		return icon_path;
    else
	if (id == IDI_MAINICON) {
		if ( _icon_resource_to_file(IDI_MAINICON, icon_path) )
            return icon_path;
	}

	return {L""};
}

void CHeygenPlugin::ClickMenuItem(int actionid)
{
    ActivationContext ctx;
    UIApplication app(nullptr, nullptr, 0);
    app.style()->loadThemesFromResource(IDT_THEMES);
    app.style()->loadStylesFromResource(IDT_STYLES);
    app.setFont({DEFAULT_FONT_NAME, 9.5});
    app.style()->setDefaultTheme(_T("Light"));

    Rect rc;
    HWND parent = GetForegroundWindow();

	if ( actionid == ID_ACTION_SETTINGS ) {
	} else
	if ( actionid == ID_ACTION_ABOUT ) {
        rc = utils::calcWindowPlacement(parent, Size(290, 180));

        AboutDialog dlg(nullptr, rc);
        if (parent)
            SetWindowLongPtr(dlg.platformWindow(), GWLP_HWNDPARENT, (LONG_PTR)parent);
        dlg.showAll();
        if (parent)
            EnableWindow(parent, FALSE);
        dlg.runDialog();
        if (parent) {
            SetForegroundWindow(parent);
            EnableWindow(parent, TRUE);
        }

	} else
	if ( actionid == ID_ACTION_GEN_VIDEO ) {
        rc = utils::calcWindowPlacement(parent, Size(1024, 768));

        MainWindow w(rc, UIWindow::RemoveSystemDecoration);
        w.setWindowTitle(_T("Heygen"));
        w.setIcon(IDI_MAINICON);
        w.setMinimumSize(800, 748);
        w.aboutToDestroySignal.connect([&app]() {
            app.exit(0);
        });

        if (parent)
            SetWindowLongPtr(w.platformWindow(), GWLP_HWNDPARENT, (LONG_PTR)parent);
        w.showAll();
        if (parent)
            EnableWindow(parent, FALSE);
        app.exec();
        if (parent) {
            SetForegroundWindow(parent);
            EnableWindow(parent, TRUE);
        }

        if ( w.result() == MainWindow::Accepted ) {
            m_priv->send_ok_callback(w.filePath());
        }
	}
}

void CHeygenPlugin::SetCallbackHandler(AsyncCallback fn, void* data)
{
	m_priv->callback_intf = fn;
    m_priv->callback_context = data;
}

void CHeygenPlugin::CleanTemporaryFiles()
{
}

