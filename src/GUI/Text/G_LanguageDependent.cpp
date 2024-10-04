#include "G_LanguageDependent.h"

#include <ZC/GUI/ZC__GUI.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <System/G_Config.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>

#include <cassert>

G_LanguageDependent::~G_LanguageDependent()
{
    ZC_ForwardListErase(ld_heirs, this);
}

void G_LanguageDependent::LanguageChanged()
{
    for (G_LanguageDependent* pld : ld_heirs) pld->VLanguageChanged_LD();
}

G_LanguageDependent::G_LanguageDependent(std::forward_list<G_LangText>&& _lang_texts)
    : lang_texts(std::move(_lang_texts))
{
    ld_heirs.emplace_front(this);
}

int G_LanguageDependent::GetLongestWidth(const std::forward_list<G_LangText>& _lang_texts)
{
    int longest_width = 0;
    for (const G_LangText& lt : _lang_texts)
    {
        int width = ZC__GUI::CalculateWstrWidth(lt.text);
        if (width > longest_width) longest_width = width;
    }
    return longest_width;
}

const std::wstring& G_LanguageDependent::GetCurrentLanguageText(std::forward_list<G_LangText>& _lang_texts)
{
    const G_LangText* pLT = ZC_Find(_lang_texts, G_Config::GetConfigData().language);
    assert(pLT);
    return pLT->text;
}