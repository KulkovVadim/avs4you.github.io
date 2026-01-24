//---------------------------------------------------------------------------

#include "CUserSettings.h"
#include "uifilesystem.h"
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>

#include "utils.h"
#include "defines.h"
//---------------------------------------------------------------------------

class CUserSettings::CUserSettingsPrivate
{
public:
	std::wstring file_path;
	nlohmann::json json_config = {};

	auto load_config() -> bool
	{
        if ( UIFileSystem::fileExists(file_path) ) {
			std::ifstream file(file_path);

			if ( file.is_open() ) {
				file >> json_config;

				file.close();
				return true;
			}
        }

		return false;
	}

	auto save_config() -> bool
	{
        if ( !json_config.empty() ) {
            std::wstring parent_dir = UIFileSystem::parentPath(file_path);
            if ( !UIFileSystem::dirExists(parent_dir) ) {
                UIFileSystem::makePath(parent_dir);
			}

			try {
				std::ofstream file(file_path);
				if ( file.is_open() ) {
					file << json_config.dump(4);
					file.close();
				} else return false;
			} catch (const std::exception& e) {
				// Error handling
				return false;
			}
        }

		return true;
	}

	auto api_key() const -> std::wstring
	{
		if ( json_config.contains("apikey") )
			return utils::utf8_to_wstr(json_config["apikey"]);

		return L"";
	}

	auto keep_api_key(const std::wstring& key) -> void
	{
		json_config["apikey"] = utils::wstr_to_utf8(key);
	}

//	auto keep_key_value(const std::string& key, const std::string& value) -> void
//	{
//		json_config[key] = value;
//	}
};

CUserSettings::CUserSettings(const std::wstring& filename)
	: m_intf(new CUserSettings::CUserSettingsPrivate)
{
	if (!filename.empty()) {
		m_intf->file_path = filename;
		m_intf->load_config();
	}
}

CUserSettings::CUserSettings()
	: m_intf(new CUserSettings::CUserSettingsPrivate)
{
}

CUserSettings::~CUserSettings()
{
	if (m_intf) {
		delete m_intf, m_intf = nullptr;
	}
}

void CUserSettings::setConfigPath(const std::wstring& path)
{
	m_intf->file_path = path;
}

void CUserSettings::load()
{
	if ( !m_intf->file_path.empty() ) {
		m_intf->load_config();
	}
}

void CUserSettings::save() const
{
	m_intf->save_config();
}

std::wstring CUserSettings::apiKey() const
{
	return m_intf->api_key();
}

void CUserSettings::keepApiKey(const std::wstring& key) const
{
	m_intf->keep_api_key(key);
}

//void CUserSettings::keepKeyValue(const String& name, const String& value)
//{
//	m_intf->keep_api_key(utils::wstr_to_utf8(name.c_str(), utils::wstr_to_utf8(value.c_str())));
//}

void CUserSettings::keepKeyValue(const std::string& key, const std::string& value)
{
	m_intf->json_config[key] = value;
}

void CUserSettings::keepKeyValue(const std::string& key, const nlohmann::json& value)
{
	m_intf->json_config[key] = value;
}

std::string CUserSettings::keyValue(const std::string& key)
{
	if ( m_intf->json_config.contains(key) ) {
		return m_intf->json_config[key].get<std::string>();
	}
	return "";
}

nlohmann::json CUserSettings::keyJsonObject(const std::string& key)
{
	if ( m_intf->json_config.contains(key) ) {
		return m_intf->json_config[key];
	}
	return {};
}

bool CUserSettings::contains(const std::string& key)
{
	return m_intf->json_config.contains(key);
}

