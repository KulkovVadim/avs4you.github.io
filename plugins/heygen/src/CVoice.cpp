//---------------------------------------------------------------------------

#include "CVoice.h"
#include "utils.h"
#include "uifilesystem.h"
//---------------------------------------------------------------------------

static void removeNewlinesAndNonBMP(std::wstring &str)
{
    str.erase(
        std::remove_if(
            str.begin(), str.end(),
            [](wchar_t ch) {
                return ch == L'\n' || IS_HIGH_SURROGATE(ch) || IS_LOW_SURROGATE(ch);
            }), str.end()
        );
}

class CVoice::CVoicePrivate {
public:
    std::wstring id,
		lang,
		gender,
		name,
		preview_audio_url,
        preview_audio_path;

	bool support_pause = false,
		support_emotion = false,
		support_interactive_avatar = false;

};

CVoice::CVoice()
	: m_intf(std::make_unique<CVoice::CVoicePrivate>())
{
}

CVoice::CVoice(const nlohmann::json& j)
	: CVoice()
{
    this->fromJson(j);
}


CVoice::~CVoice()
{
    if (!m_intf->preview_audio_path.empty() && UIFileSystem::fileExists(m_intf->preview_audio_path))
        UIFileSystem::removeFile(m_intf->preview_audio_path);
}

bool CVoice::fromJson(nlohmann::json j)
{
	if ( !j.empty() ) {
        if (j.contains("voice_id") && !j["voice_id"].is_null()) {
            m_intf->id = utils::utf8_to_wstr(j["voice_id"].get<std::string>());
        }

        if (j.contains("name") && !j["name"].is_null()) {
            auto name = utils::utf8_to_wstr(j["name"].get<std::string>());
            removeNewlinesAndNonBMP(name);
            m_intf->name = name;
        }

        if (j.contains("language") && !j["language"].is_null()) {
            m_intf->lang = utils::utf8_to_wstr(j["language"].get<std::string>());
        }

        if (j.contains("preview_audio") && !j["preview_audio"].is_null()) {
            m_intf->preview_audio_url = utils::utf8_to_wstr(j["preview_audio"].get<std::string>());
        }

        if (j.contains("gender") && !j["gender"].is_null()) {
            m_intf->gender = utils::utf8_to_wstr(j["gender"].get<std::string>());
        }

//		std::string gender = j["gender"].get<std::string>();
//		if ( gender == "female" ) {
//			m_intf->gender = Gender::Female;
//		} else
//		if ( gender == "female" ) {
//			m_intf->gender = Gender::Male;
//		} else m_intf->gender = Gender::Unknown;

        if (j.contains("support_pause") && !j["support_pause"].is_null()) {
            m_intf->support_pause = j["support_pause"];
        }

        if (j.contains("emotion_support") && !j["emotion_support"].is_null()) {
            m_intf->support_emotion = j["emotion_support"];
        }

        if (j.contains("support_interactive_avatar") && !j["support_interactive_avatar"].is_null()) {
            m_intf->support_interactive_avatar = j["support_interactive_avatar"];
        }

        return true;
	}

    return false;
}

std::wstring CVoice::id() const
{
	return m_intf->id;
}

std::wstring CVoice::name() const
{
	return m_intf->name;
}

std::wstring CVoice::gender() const
{
	return m_intf->gender;
}

std::wstring CVoice::language() const
{
	return m_intf->lang;
}

bool CVoice::supportPause() const
{
	return m_intf->support_pause;
}

bool CVoice::supportEmotion() const
{
	return m_intf->support_emotion;
}

bool CVoice::supportInteractiveAvatar() const
{
	return m_intf->support_interactive_avatar;
}

std::wstring CVoice::previewAudioUrl() const
{
    return m_intf->preview_audio_url;
}

std::wstring CVoice::previewAudioPath() const
{
    return m_intf->preview_audio_path;
}

void CVoice::setPreviewAudioPath(const std::wstring &path) const
{
    m_intf->preview_audio_path = path;
}

