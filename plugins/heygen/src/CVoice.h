//---------------------------------------------------------------------------

#ifndef CVoiceH
#define CVoiceH
//---------------------------------------------------------------------------

#include <nlohmann/json.hpp>

class CVoice {
public:
	CVoice();
	CVoice(const nlohmann::json& j);
	~CVoice();

    std::wstring id() const;
    std::wstring name() const;
    std::wstring gender() const;
    std::wstring language() const;
    bool supportPause() const;
	bool supportEmotion() const;
	bool supportInteractiveAvatar() const;
    std::wstring previewAudioUrl() const;
    std::wstring previewAudioPath() const;
    void setPreviewAudioPath(const std::wstring &path) const;

	bool fromJson(nlohmann::json);
private:
	class CVoicePrivate;
	std::unique_ptr<CVoicePrivate> m_intf;
};

#endif
