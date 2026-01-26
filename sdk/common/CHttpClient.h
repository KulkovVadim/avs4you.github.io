//---------------------------------------------------------------------------

#ifndef CHttpClientH
#define CHttpClientH
//---------------------------------------------------------------------------

#include <string>
#include <memory>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <vector>

using VectorHeaders = std::vector<std::pair<std::wstring, std::wstring>>;
using RequestCallback = std::function<void(int, std::wstring, int)>;
using RequestCallbackA = std::function<void(int, std::string, int)>;
using DownloadCallback = std::function<void(int, std::wstring, std::wstring)>;

class CHttpClient {
private:

public:
	explicit CHttpClient(const std::wstring& url);
	explicit CHttpClient();
	~CHttpClient();

	void setRequestHeaders(const VectorHeaders&);
	void setRequestJsonData(const std::wstring&);
	void setRequestJsonData(const std::string&);
	void setRequestFormData(const std::vector<std::pair<std::wstring, std::wstring>>&);

	void request(const std::wstring& url, int action = 0);
	void download(const std::wstring& url, const std::wstring& ext, const std::wstring& id = L"") const;
    void setTempPath(const std::wstring&);

	void setOnRequestCompleteCallback(RequestCallback callback);
	void setOnRequestACompleteCallback(RequestCallbackA callback);
	void setOnDownloadCompleteCallback(DownloadCallback callback);

	static std::wstring codeShort(int);
private:
	class CHttpClientPrivate;
	CHttpClientPrivate * m_priv;
};

#endif
