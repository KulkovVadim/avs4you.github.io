#ifndef ISLTRANSLATOR_H
#define ISLTRANSLATOR_H

#include <unordered_map>
#include <string>

#define _TR(str) ISLTranslator::instance().tr(#str)

using std::wstring;

typedef std::unordered_map<wstring, wstring> LocaleMap;
typedef std::unordered_map<wstring, LocaleMap> TranslationsMap;


class ISLTranslator
{
public:
    ISLTranslator(const ISLTranslator&) = delete;
    ISLTranslator& operator=(const ISLTranslator&) = delete;
    static ISLTranslator& instance();

    void init(const wstring &lang, int resourceId);
    wstring tr(const char*);
    void setLanguage(const wstring &lang);

private:
    ISLTranslator();
    ~ISLTranslator();

    TranslationsMap translMap;
    wstring langName;
    bool    is_translations_valid;

    enum TokenType {
        TOKEN_BEGIN_DOCUMENT = 0,
        TOKEN_END_DOCUMENT,
        TOKEN_BEGIN_STRING_ID,
        TOKEN_END_STRING_ID,
        TOKEN_BEGIN_LOCALE,
        TOKEN_END_LOCALE,
        TOKEN_BEGIN_VALUE,
        TOKEN_END_VALUE
    };
};

#endif // ISLTRANSLATOR_H
