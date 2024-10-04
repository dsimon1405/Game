#pragma once

#include "G_LanguageDependent.h"

template <typename TGUIText>
class G_GUI_TextObjKeeper : public G_LanguageDependent, public TGUIText
{
public:
    G_GUI_TextObjKeeper(TGUIText&& guiText, std::forward_list<G_LangText>&& _lang_texts);

private:
    void VLanguageChanged_LD() override;
};

template <typename TGUIText>
G_GUI_TextObjKeeper<TGUIText>::G_GUI_TextObjKeeper(TGUIText&& guiText, std::forward_list<G_LangText>&& _lang_texts)
    : G_LanguageDependent(std::move(_lang_texts)),
    TGUIText(std::move(guiText))
{}

template <typename TGUIText>
void G_GUI_TextObjKeeper<TGUIText>::VLanguageChanged_LD()
{
    this->UpdateText(this->GetCurrentLanguageText(this->lang_texts), true);
}