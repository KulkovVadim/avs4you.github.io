#ifndef CHeygenPluginH
#define CHeygenPluginH

#include "CBase.h"
#include <Windows.h>
#include <string>
#include <memory>

class CHeygenPlugin
{
public:
	CHeygenPlugin();
    virtual ~CHeygenPlugin();

	std::wstring Id() const;
    std::wstring Icon() const;
    Plugins::PluginType Type() const;
	void ReleasePluginString(WCHAR* ptr) const;

	void SetLanguage(const wchar_t*);
	std::wstring GetMenuForContext(Plugins::ContextType type) const;
	std::wstring GetPluginMenu() const;
	std::wstring GetIconById(int id) const;

	virtual void ClickMenuItem(int actionid);

    void SetCallbackHandler(AsyncCallback, void*);
    void CleanTemporaryFiles();
private:
	class CHeygenPluginPrivate;
	std::unique_ptr<CHeygenPluginPrivate> m_priv;
};


#endif
