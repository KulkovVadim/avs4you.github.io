#include "utils.h"
#include <codecvt>

namespace NSStringUtils
{
	void replace(std::wstring& text, const std::wstring& replaceFrom, const std::wstring& replaceTo)
	{
		size_t posn = 0;
		while (std::wstring::npos != (posn = text.find(replaceFrom, posn)))
		{
			text.replace(posn, replaceFrom.length(), replaceTo);
			posn += replaceTo.length();
		}
	}
	void replace(std::string& text, const std::string& replaceFrom, const std::string& replaceTo)
	{
		size_t posn = 0;
		while (std::string::npos != (posn = text.find(replaceFrom, posn)))
		{
			text.replace(posn, replaceFrom.length(), replaceTo);
			posn += replaceTo.length();
		}
	}

	std::wstring utf8_to_wstring(const std::string& str)
	{
		try
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			return conv.from_bytes(str);
		}
		catch (...)
		{
			std::wstring result;
			result.reserve(str.size());
			for (char c : str)
				result += static_cast<wchar_t>(c);
			return result;
		}
	}
	std::string wstring_to_utf8(const std::wstring& wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.to_bytes(wstr);

		try
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			return conv.to_bytes(wstr);
		}
		catch (...)
		{
			std::string result;
			result.reserve(wstr.size());
			for (wchar_t c : wstr)
				result += static_cast<char>(c);
			return result;
		}
	}

	std::string unescapeJson(const std::string& input)
	{
		std::string result;
		result.reserve(input.size());

		for (size_t i = 0; i < input.size(); ++i)
		{
			if (input[i] == '\\' && i + 5 < input.size() && input[i + 1] == 'u')
			{
				std::string code = input.substr(i + 2, 4);
				int charCode = std::stoi(code, nullptr, 16);
				result += static_cast<char>(charCode);
				i += 5;
			}
			else
			{
				result += input[i];
			}
		}

		return result;
	}
} // namespace NSStringUtils
