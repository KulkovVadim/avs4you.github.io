#pragma once
#include "./../3dparty/nlohmann/json/single_include/nlohmann/json.hpp"
#include <windows.h>
#include <codecvt>
#include <locale>

class CTranslateManager
{
private:
	std::string m_content = "";
	std::string m_lang = "";
	nlohmann::json m_obj;

public:
	CTranslateManager(HMODULE hModule, const int& resourceID);
	~CTranslateManager();

	bool SetLang(const std::string& lang);
	std::wstring Translate(const std::wstring& name);
};

class CTranslate
{
private:
	CTranslateManager* m_manager = nullptr;
	CTranslate() = default;

public:
	~CTranslate();

	void Init(HMODULE hModule, int resourceID);
	CTranslateManager* GetManager();

	static CTranslate& GetInstance();
};