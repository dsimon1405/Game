#pragma once

#include "G_LangText.h"
#include <ZC/GUI/Backend/Text/ZC_GUI_Font.h>

#include <forward_list>

class G_LanguageDependent
{
public:
    virtual ~G_LanguageDependent();

    static void LanguageChanged();

protected:
    std::forward_list<G_LangText> lang_texts;
    const ZC_GUI_Font* pFont;

    G_LanguageDependent(std::forward_list<G_LangText>&& _lang_texts);

    static int GetLongestWidth(const ZC_GUI_Font* _pFont, const std::forward_list<G_LangText>& _lang_texts);
    static const std::wstring& GetCurrentLanguageText(std::forward_list<G_LangText>& _lang_texts);

private:
    static inline std::forward_list<G_LanguageDependent*> ld_heirs;

    virtual void VLanguageChanged_LD() = 0;
};