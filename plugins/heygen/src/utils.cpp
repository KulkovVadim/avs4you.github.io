
#include "utils.h"
#include <codecvt>
#include <ctime>
// #include <openssl/bio.h>
// #include <openssl/evp.h>
#include "uiutils.h"
#include "uiwidget.h"
#include "webp/decode.h"

namespace utils {
    std::string wstr_to_utf8(const std::wstring& wstr)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

    std::wstring utf8_to_wstr(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    std::wstring toLowerCase(const std::wstring &str)
    {
        std::wstring result = str;
        if (!result.empty())
            CharLowerBuffW(&result[0], static_cast<DWORD>(result.size()));
        return result;
    }

    std::string generate_unique_filename(const std::string& extension)
    {
        auto now = std::time(nullptr);
        auto timestamp = static_cast<long long>(now);
        return "avs_" + std::to_string(timestamp) + extension;
    }

    wchar_t* wstr_to_ptr(const std::wstring& s)
    {
        size_t len = s.length();
        wchar_t* result = new wchar_t[len + 1];
        if ( len > 0 ) {
            std::wcsncpy(result, s.c_str(), len + 1);
        }
        result[len] = L'\0';
        return result;
    }

    void release_wstr_ptr(wchar_t* ptr)
    {
        delete[] ptr;
    }

    bool loadWebP(const std::wstring &filename, std::vector<uint8_t>& out_pixels, int& width, int& height)
    {
        auto _filename = wstr_to_utf8(filename);
        FILE* file = fopen(_filename.c_str(), "rb");
        if (!file) return false;

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::vector<uint8_t> buffer(size);
        if (fread(buffer.data(), 1, size, file) != static_cast<size_t>(size)) {
            fclose(file);
            return false;
        }
        fclose(file);

        int w = 0, h = 0;
        uint8_t* rgba = WebPDecodeRGBA(buffer.data(), size, &w, &h);
        if (!rgba) return false;

        width = w;
        height = h;
        size_t pixel_count = w * h * 4;
        out_pixels.assign(rgba, rgba + pixel_count);

        WebPFree(rgba);
        return true;
    }

    HMODULE getCurrentModule()
    {
        HMODULE hModule = NULL;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                         (LPCTSTR)getCurrentModule, &hModule);
        return hModule;
    }

    Rect calcWindowPlacement(HWND parent, Size defaultSize)
    {
        Rect rc(100, 100, defaultSize.width, defaultSize.height);
        RECT targetRc = {};
        if (parent) {
            GetWindowRect(parent, &targetRc);
        } else {
            targetRc = {100, 100, 100 + defaultSize.width, 100 + defaultSize.height};
        }

        double screenDpi = UIScreen::dpiAtRect(targetRc);
        rc.width *= screenDpi;
        rc.height *= screenDpi;

        if (parent) {
            rc.x = (targetRc.left + targetRc.right - rc.width) / 2;
            rc.y = (targetRc.top + targetRc.bottom - rc.height) / 2;
        } else {
            rc.x *= screenDpi;
            rc.y *= screenDpi;
        }
        return rc;
    }

    Rect calcWindowPlacement(UIWidget *parent, Size defaultSize)
    {
        Rect rc(100, 100, defaultSize.width, defaultSize.height);
        double dpi = 1.0;
        if (parent) {
            dpi = parent->dpiRatio();
        } else {
            RECT targetRc = {100, 100, 100 + defaultSize.width, 100 + defaultSize.height};
            dpi = UIScreen::dpiAtRect(targetRc);
        }

        rc.width *= dpi;
        rc.height *= dpi;

        if (parent) {
            Point wndPos = parent->pos();
            Size wndSize = parent->size();
            rc.x = wndPos.x + (wndSize.width - rc.width) / 2;
            rc.y = wndPos.y + (wndSize.height - rc.height) / 2;
        } else {
            rc.x *= dpi;
            rc.y *= dpi;
        }
        return rc;
    }

	namespace crypt {
//		bool encrypt_data(const std::string& data, std::string& encrypted)
//		{
//			DATA_BLOB dataIn, dataOut;
//			BOOL result;
//
//			dataIn.pbData = (BYTE*)data.c_str();
//			dataIn.cbData = data.size();
//
//			result = CryptProtectData(&dataIn, L"UserData", NULL,NULL,NULL,0, &dataOut);
//			if (result) {
//				encrypted.assign((char*)dataOut.pbData, dataOut.cbData);
//				LocalFree(dataOut.pbData);
//				return true;
//			}
//
//			return false;
//		}

//		bool decrypt_data(const std::string& encryptedData, std::string& decryptedData)
//		{
//			DATA_BLOB dataIn, dataOut;
//			BOOL result;
//
//			dataIn.pbData = (BYTE*)encryptedData.c_str();
//			dataIn.cbData = encryptedData.size();
//
//			result = CryptUnprotectData(&dataIn,NULL,NULL,NULL,NULL,0,&dataOut);
//			if (result) {
//				decryptedData.assign((char*)dataOut.pbData, dataOut.cbData);
//				LocalFree(dataOut.pbData);
//				return true;
//			}
//
//			return false;
//		}

        std::vector<unsigned char> base64_decode(const std::string& base64_str)
		{
			std::vector<unsigned char> decoded;
			return decoded;
		}
	}
}
