//---------------------------------------------------------------------------

#include "CAvatar.h"
#include <nlohmann/json.hpp>
#include "utils.h"
#include "uifilesystem.h"
//---------------------------------------------------------------------------

class CAvatar::CAvatarPrivate {
public:
    std::wstring id,
			name;
	CAvatar::Gender gender = CAvatar::Gender::Unknown;
	bool premium;
    std::wstring preview_image_path,
			preview_image_url,
            preview_video_url;

public:
	CAvatarPrivate() {}
	~CAvatarPrivate() {}
};

CAvatar::CAvatar()
	: m_intf(std::make_unique<CAvatar::CAvatarPrivate>())
{
}

CAvatar::CAvatar(const nlohmann::json& j)
	: CAvatar()
{
	this->fromJson(j);
}

CAvatar::~CAvatar()
{
    if (!m_intf->preview_image_path.empty() && UIFileSystem::fileExists(m_intf->preview_image_path))
        UIFileSystem::removeFile(m_intf->preview_image_path);
}

void CAvatar::parseJson(const std::wstring& jsonstr)
{
//	if ( !jsonstr.IsEmpty() ) {
//		nlohmann::json a = nlohmann::json::parse(utils::wstr_to_utf8(jsonstr.c_str()));
//
//		m_intf->id = utils::utf8_to_wstr(a["id"]);
//		m_intf->name = utils::utf8_to_wstr(a["name"]);
//
//		std::string gender = a["gender"];
//		if ( gender == "female" ) {
//			m_intf->gender = Gender::Female;
//		} else
//		if ( gender == "female" ) {
//			m_intf->gender = Gender::Male;
//		} else m_intf->gender = Gender::Unknown;
//	}
}

void CAvatar::fromJson(nlohmann::json j)
{
	if ( !j.empty() ) {
		m_intf->id = utils::utf8_to_wstr(j["avatar_id"].get<std::string>());
		m_intf->name = utils::utf8_to_wstr(j["avatar_name"].get<std::string>());
		m_intf->preview_image_url = utils::utf8_to_wstr(j["preview_image_url"].get<std::string>());
		m_intf->preview_video_url = utils::utf8_to_wstr(j["preview_video_url"].get<std::string>());

		std::string gender = j["gender"].get<std::string>();
        std::transform(gender.begin(), gender.end(), gender.begin(),
        	[](unsigned char c) { return std::tolower(c); }
        );
		if ( gender == "female" ) {	
			m_intf->gender = Gender::Female;
		} else
        if ( gender == "male" ) {
			m_intf->gender = Gender::Male;
		} else m_intf->gender = Gender::Unknown;

		m_intf->premium = j["premium"];
	}
}

std::wstring CAvatar::id() const
{
	return m_intf->id;
}

std::wstring CAvatar::name() const
{
	return m_intf->name;
}

CAvatar::Gender CAvatar::gender() const
{
	return m_intf->gender;
}

std::wstring CAvatar::previewImageUrl() const
{
	return m_intf->preview_image_url;
}

std::wstring CAvatar::previewVideoUrl() const
{
	return m_intf->preview_video_url;
}

bool CAvatar::premium()
{
	return m_intf->premium;
}

std::wstring CAvatar::previewImagePath() const
{
	return m_intf->preview_image_path;
}

void CAvatar::setPreviewImagePath(const std::wstring& path)
{
    m_intf->preview_image_path = path;
}
