
#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <string>
#include <vector>
#include "uicommon.h"

class UIWidget;
namespace utils {
    std::string wstr_to_utf8(const std::wstring& wstr);
    std::wstring utf8_to_wstr(const std::string& str);
    std::wstring toLowerCase(const std::wstring &str);
    std::string generate_unique_filename(const std::string& extension);
    wchar_t* wstr_to_ptr(const std::wstring& s);
    void release_wstr_ptr(wchar_t* ptr);
    bool loadWebP(const std::wstring &filename, std::vector<uint8_t>& out_pixels, int& width, int& height);
    HMODULE getCurrentModule();
    Rect calcWindowPlacement(HWND parent, Size defaultSize);
    Rect calcWindowPlacement(UIWidget *parent, Size defaultSize);

	namespace crypt {
//		bool encrypt_data(const std::string& data, std::string& encrypted);
//		bool decrypt_data(const std::string& encryptedData, std::string& decryptedData);
        std::vector<unsigned char> base64_decode(const std::string& base64_str);
	}
}

#endif
