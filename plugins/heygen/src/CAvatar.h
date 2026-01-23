//---------------------------------------------------------------------------

#ifndef CAvatarH
#define CAvatarH
//---------------------------------------------------------------------------

#include <nlohmann/json.hpp>

class CAvatar {
public:
	CAvatar();
	CAvatar(const nlohmann::json& j);
	~CAvatar();

	enum class Gender {
        Unknown = 0,
		Male,
		Female
	};

    std::wstring id() const;
    std::wstring name() const;
	Gender gender() const;
    std::wstring previewImageUrl() const;
    std::wstring previewVideoUrl() const;
	bool premium();
//	  "type": null,
//	  "tags": null

    std::wstring previewImagePath() const;
    void setPreviewImagePath(const std::wstring& path);

    void parseJson(const std::wstring&);
    void fromJson(nlohmann::json);
private:
	class CAvatarPrivate;
    std::unique_ptr<CAvatarPrivate> m_intf;
};

#endif
