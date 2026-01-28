//---------------------------------------------------------------------------

#pragma hdrstop

#include "CHttpClient.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "libcurl.lib"

#include "curl/curl.h"
#include <codecvt>
#include <fstream>
#include <thread>
#include <Rpc.h>
#include "HttpStatusCodes.h"
//#include "utils.h"
//#include "defines.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Rpcrt4.lib")

namespace {
class CurlGlobalGuard {
public:
	CurlGlobalGuard() { curl_global_init(CURL_GLOBAL_ALL); }
	~CurlGlobalGuard() { curl_global_cleanup(); }
};

struct ResponseData {
	std::string data;
	size_t write(const char* buffer, size_t size)
	{
		data.append(buffer, size);
		return size;
	}
};

// Callback for libcurl
size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	ResponseData* response = static_cast<ResponseData*>(userdata);
	return response->write(ptr, size * nmemb);
}

size_t stream_write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
	stream->write(static_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
}

namespace utils {
	std::string wstr_to_utf8(const std::wstring& wstr) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(wstr);
	}

	std::wstring utf8_to_wstr(const std::string& str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}

    std::wstring generate_unique_filename(const std::wstring& extension) {
        UUID uuid;
        RPC_WSTR wszUuid = NULL;
        if (UuidCreate(&uuid) != RPC_S_OK || UuidToStringW(&uuid, &wszUuid) != RPC_S_OK)
            throw std::runtime_error("Failed to generate UUID");

        std::wstring result(L"avs_");
        result.append((const wchar_t*)(wszUuid));
    	result.append(extension);
        RpcStringFree(&wszUuid);
		return result;
	}

    std::wstring tempPath() {
        WCHAR buff[MAX_PATH + 1] = {0};
        DWORD res = GetTempPath(MAX_PATH, buff);
        if (res != 0 && res <= MAX_PATH) {
            return buff;
        }
        return {};
    }
}

struct download_result {
	bool success;
    std::wstring filepath;
	std::string error;
};

std::string get_extension_from_content_type(const std::string& contentType)
{
	if (contentType.find("image/jpeg") != std::string::npos) return ".jpg";
	if (contentType.find("image/png") != std::string::npos) return ".png";
	if (contentType.find("image/gif") != std::string::npos) return ".gif";
	if (contentType.find("image/webp") != std::string::npos) return ".webp";
	return ".jpg";
}
}

class CHttpClient::CHttpClientPrivate {
public:
	std::wstring request_url;
	std::string request_json_body;
    std::wstring temp_path;
	std::vector<std::pair<std::wstring, std::wstring>> request_form_body;
	VectorHeaders request_headers;
	RequestCallback request_callback = nullptr;
	RequestCallbackA request_callback_a = nullptr;
	DownloadCallback download_callback = nullptr;

    CHttpClientPrivate()
    {
        temp_path = utils::tempPath();
    }

    auto download_image(const std::string& url, const std::wstring& ext, const std::wstring& id) -> void
	{
        auto tempFilePath = temp_path + utils::generate_unique_filename(ext);

		std::thread _thread{[=] () {
            download_result result{false, L"", ""};
			CurlGlobalGuard curlGuard;

			try {
                std::ofstream outFile(tempFilePath, std::ios::binary);
				if (!outFile.is_open()) {
					throw std::runtime_error("Error on create temp file name");
				}

				std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
				if (!curl) {
					throw std::runtime_error("CURL init error");
				}

//				struct curl_slist* curl_headers = nullptr;
//				std::string param = "authorization: Bearer 9117f5f366fd26e7d371208d68456cc1f91116fcaa08a3aedb066479429a866d";
//				curl_headers = curl_slist_append(curl_headers, param.c_str());
//				curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, curl_headers);

				curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, stream_write_data);
				curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &outFile);
				curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // for Windows

				CURLcode res = curl_easy_perform(curl.get());
				outFile.close();

				if (res != CURLE_OK) {
                    OutputDebugStringA(curl_easy_strerror(res));
					throw std::runtime_error(curl_easy_strerror(res));
//                    curl_easy_cleanup(curl);
				}

				// get Content-Type
				char* contentType = nullptr;
				curl_easy_getinfo(curl.get(), CURLINFO_CONTENT_TYPE, &contentType);

//				std::string extension = contentType ? get_extension_from_content_type(contentType) : ".jpg";

//				auto finalPath = tempPath / (tempFilename + extension);
//				std::filesystem::rename(tempFilePath, finalPath);
//				result.filepath = finalPath.string();

				result.success = true;
                result.filepath = tempFilePath;
			}
			catch (const std::exception& e) {
				result.success = false;
				result.error = e.what();
			}

			if (result.success) {
				if ( download_callback ) {
                    download_callback(0, result.filepath, id);
				}
			}
		}};
		_thread.detach();
	}

	auto make_request(const std::wstring& url, int action) -> void
	{
		std::thread _thread{[=](){
			CurlGlobalGuard curl_guard;
			ResponseData response;

			std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
			if (!curl) {
				throw std::runtime_error("Failed to initialize CURL");
			}

/**/
			curl_easy_setopt(curl.get(), CURLOPT_URL, utils::wstr_to_utf8(url).c_str());
			curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);

//			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
//			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

			/**
				fill request headers
			*/
			struct curl_slist *curl_headers = nullptr;
			if ( !this->request_headers.empty() ) {
				std::string param;
				for (auto& h : this->request_headers) {
					param = utils::wstr_to_utf8(h.first + L": " + h.second);
					curl_headers = curl_slist_append(curl_headers, param.c_str());
				}

				curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, curl_headers);
			}

			/**
				set request body
			*/
//			std::unique_ptr<curl_mime, decltype(&curl_mime_free)> mime(nullptr, curl_mime_free);
			struct curl_mime *mime = nullptr;
			struct curl_mimepart *field = nullptr;
			if ( this->request_json_body.length() ) {
				curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, this->request_json_body.c_str());
			} else
			if ( !this->request_form_body.empty() ) {
//				mime = std::make_unique<curl_mime>(curl_mime_init(curl.get()));
				mime = curl_mime_init(curl.get());

				std::string name, data;
				for (auto b: this->request_form_body) {
					name = utils::wstr_to_utf8(b.first);
					data = utils::wstr_to_utf8(b.second);

					field = curl_mime_addpart(mime);
					curl_mime_name(field, name.c_str());
					curl_mime_data(field, data.c_str(), CURL_ZERO_TERMINATED);
				}

				curl_easy_setopt(curl.get(), CURLOPT_MIMEPOST, mime);
			}

			// for windows skip SSL verifying
			curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L);

/**/

			CURLcode res = curl_easy_perform(curl.get());
			if (res != CURLE_OK) {
				throw std::runtime_error(curl_easy_strerror(res));
			}

			long http_code = 0;
			curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);
//			if (http_code != 200) {
//				OutputDebugString(response.data.c_str());
//				throw std::runtime_error("HTTP error: " + std::to_string(http_code));
//			}

			if ( curl_headers ) {
				curl_slist_free_all(curl_headers);
			}
			if ( mime ) {
				curl_mime_free(mime);
			}


			if ( request_callback_a ) {
                // OutputDebugStringA(response.data.c_str());
				request_callback_a(http_code, response.data, action);
			} else
			if ( request_callback ) {
                // OutputDebugStringA(response.data.c_str());
				request_callback(http_code, utils::utf8_to_wstr(response.data), action);
			}
		}};
		_thread.detach();
	}
};

/*
*   class definition
*/

CHttpClient::CHttpClient(const std::wstring& url)
	: m_priv(new CHttpClient::CHttpClientPrivate)
{
	m_priv->request_url = url;

//	OutputDebugStringW(L"request url");
//	OutputDebugStringW(request_url);
	std::wstring s = url;
}

CHttpClient::CHttpClient()
	: m_priv(new CHttpClient::CHttpClientPrivate)
{
}

CHttpClient::~CHttpClient()
{
    delete m_priv, m_priv = nullptr;
}

void CHttpClient::setRequestHeaders(const VectorHeaders& headers)
{
	m_priv->request_headers = headers;
}

void CHttpClient::setRequestJsonData(const std::wstring& jsonstr)
{
	m_priv->request_json_body = utils::wstr_to_utf8(jsonstr);
}

void CHttpClient::setRequestJsonData(const std::string& jsonstr)
{
	m_priv->request_json_body = jsonstr;
}

void CHttpClient::setRequestFormData(const std::vector<std::pair<std::wstring, std::wstring>>& fields)
{
	m_priv->request_form_body = fields;
}

void CHttpClient::setTempPath(const std::wstring& path)
{
    if (!path.empty()) {
        m_priv->temp_path = path;
        std::replace(m_priv->temp_path.begin(), m_priv->temp_path.end(), L'/', L'\\');
        if (m_priv->temp_path.back() != L'\\') {
            m_priv->temp_path += L'\\';
        }
    }
}

void CHttpClient::request(const std::wstring& url, int action)
{
	m_priv->make_request(url, action);
}

void CHttpClient::download(const std::wstring& url, const std::wstring& ext, const std::wstring& id) const
{
	if ( url.length() ) {
        m_priv->download_image(utils::wstr_to_utf8(url), ext, id);
	}
}

void CHttpClient::setOnRequestCompleteCallback(RequestCallback callback)
{
	m_priv->request_callback = callback;
}

void CHttpClient::setOnRequestACompleteCallback(RequestCallbackA callback)
{
	m_priv->request_callback_a = callback;
}

void CHttpClient::setOnDownloadCompleteCallback(DownloadCallback callback)
{
	m_priv->download_callback = callback;
}

std::wstring CHttpClient::codeShort(int code)
{
    std::string phrase{HttpStatus::reasonPhrase(code)};
    return utils::utf8_to_wstr(phrase);
}

