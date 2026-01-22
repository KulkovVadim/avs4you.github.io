//---------------------------------------------------------------------------

#ifndef CPluginBaseH
#define CPluginBaseH
//---------------------------------------------------------------------------

#include "CBase.h"
#include <string>

class CPluginBase {
public:
	virtual ~CPluginBase() {}

    virtual void SetModuleHandle(HINSTANCE) = 0;
    virtual bool CheckPlugin() = 0;

	virtual std::wstring Id() const = 0;
	virtual std::wstring Version() const = 0;
	virtual std::wstring Name() const = 0;
    virtual std::wstring Icon() const = 0;
    virtual std::wstring Info() const = 0;
    virtual Plugins::PluginType Type() const = 0;
    virtual bool IsApplicationSupported(int) const = 0;
	virtual void ReleasePluginString(wchar_t* ptr) const = 0;

public:
};

#endif
