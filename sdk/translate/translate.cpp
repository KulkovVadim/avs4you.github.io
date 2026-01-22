#include "translate.h"
#include "../common/utils.h"

CTranslateManager::CTranslateManager(HMODULE hModule, const int& resourceID)
{
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
	if (hResource)
	{
		DWORD size = SizeofResource(hModule, hResource);
		HGLOBAL hLoaded = LoadResource(hModule, hResource);
		if (hLoaded)
		{
			void* pLocked = LockResource(hLoaded);
			if (pLocked)
			{
				m_content = std::string(reinterpret_cast<char*>(pLocked), size);
			}
		}
	}
}

CTranslateManager::~CTranslateManager()
{
}

bool CTranslateManager::SetLang(const std::string& lang)
{
	m_lang = lang;
	m_obj = nlohmann::json::object();
	if (m_content.empty())
		return false;

	nlohmann::json param = nlohmann::json::parse(m_content);
	if (!param.contains(m_lang) || !param[m_lang].is_object())
		return false;

	m_obj = param[m_lang];
	return true;
}

std::wstring CTranslateManager::Translate(const std::wstring& name)
{
	std::string nameA = NSStringUtils::wstring_to_utf8(name);
	if (m_obj.contains(nameA) && m_obj[nameA].is_string())
		return NSStringUtils::utf8_to_wstring((std::string)m_obj[nameA]);
	return name;
}

CTranslate::~CTranslate()
{
	if (m_manager)
		delete m_manager;
}

void CTranslate::Init(HMODULE hModule, int resourceID)
{
	m_manager = new CTranslateManager(hModule, resourceID);
}

CTranslateManager* CTranslate::GetManager()
{
	return m_manager;
}

CTranslate& CTranslate::GetInstance()
{
	static CTranslate g_instance;
	return g_instance;
}
