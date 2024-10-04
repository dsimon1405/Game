#pragma once

#include "G_LangText.h"

#include <forward_list>

class G_LanguageDependent
{
public:
    virtual ~G_LanguageDependent();

    static void LanguageChanged();

protected:
    std::forward_list<G_LangText> lang_texts;

    G_LanguageDependent(std::forward_list<G_LangText>&& _lang_texts);

    int GetLongestWidth(const std::forward_list<G_LangText>& _lang_texts);
    const std::wstring& GetCurrentLanguageText(std::forward_list<G_LangText>& _lang_texts);

private:
    static inline std::forward_list<G_LanguageDependent*> ld_heirs;

    virtual void VLanguageChanged_LD() = 0;
};