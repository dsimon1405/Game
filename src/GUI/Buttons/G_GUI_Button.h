#pragma once

#include <GUI/Text/G_LangText.h>
#include <Sound/G_SystemSound.h>
#include <ZC/GUI/Backend/Text/ZC_GUI_Font.h>

#include <forward_list>

class G_GUI_Button
{
public:
    G_GUI_Button(bool _use_focuse_changed_sound);
    G_GUI_Button(G_GUI_Button&) = delete;
    G_GUI_Button(G_GUI_Button&&) = delete;
    virtual ~G_GUI_Button() = default;

    void ButtonFocusChanged(bool is_on);

    static float CalculateButtonWidth(const ZC_GUI_Font* pFont, const std::forward_list<G_LangText>& _lang_texts);
    static float GetLongestName(const ZC_GUI_Font* pFont, const std::forward_list<G_LangText>& _lang_texts);

private:
    static inline ZC_uptr<G_SystemSound> up_ss_button_focused;
    
    bool use_focuse_changed_sound;
};