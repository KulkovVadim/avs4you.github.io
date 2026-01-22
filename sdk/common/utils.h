#pragma once
#include <string>

namespace NSStringUtils
{
	void replace(std::wstring& text, const std::wstring& replaceFrom, const std::wstring& replaceTo);
	void replace(std::string& text, const std::string& replaceFrom, const std::string& replaceTo);

	std::wstring utf8_to_wstring(const std::string& str);
	std::string wstring_to_utf8(const std::wstring& wstr);
	
	std::string unescapeJson(const std::string& input);
}