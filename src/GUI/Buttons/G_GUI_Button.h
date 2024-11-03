#pragma once

#include <GUI/Text/G_LangText.h>
#include <Sound/G_SystemSound.h>

#include <forward_list>

class G_GUI_Button
{
public:
    G_GUI_Button(bool _use_focuse_changed_sound);
    G_GUI_Button(G_GUI_Button&) = delete;
    G_GUI_Button(G_GUI_Button&&) = delete;
    virtual ~G_GUI_Button() = default;

    void ButtonFocusChanged(bool is_on);

    static float CalculateButtonWidth(const std::forward_list<G_LangText>& _lang_texts);
    static float GetLongestName(const std::forward_list<G_LangText>& _lang_texts);

private:
    static inline G_SystemSound ss_button_focused;
    
    bool use_focuse_changed_sound;
};