//---------------------------------------------------------------------------

#ifndef CUserSettingsH
#define CUserSettingsH
//---------------------------------------------------------------------------

#include <string>
#include "nlohmann/json.hpp"

class CUserSettings
{
public:
	CUserSettings();
	CUserSettings(const std::wstring&);
	~CUserSettings();

	void setConfigPath(const std::wstring&);
	void load();
	void save() const;
	std::wstring apiKey() const;
	void keepApiKey(const std::wstring& key) const;

//	void keepKeyValue(const String& name, const String& value);
	void keepKeyValue(const std::string& key, const std::string& value);
	void keepKeyValue(const std::string& key, const nlohmann::json& value);
	std::string keyValue(const std::string& key);
	nlohmann::json keyJsonObject(const std::string& key);

    bool contains(const std::string& key);

private:
	class CUserSettingsPrivate;
	CUserSettingsPrivate * m_intf;
};

#endif
