#include "G_GUI_Button.h"

#include <ZC/GUI/ZC__GUI.h>
#include <ZC/Audio/ZC_Sounds.h>
#include <Sound/G_SoundName.h>

G_GUI_Button::G_GUI_Button(bool _use_focuse_changed_sound)
    : use_focuse_changed_sound(_use_focuse_changed_sound)
{
    if (!(ss_button_focused.upSound) && use_focuse_changed_sound) ss_button_focused = G_SystemSound(G_SN__gui_button_focused, 100.f);
}

void G_GUI_Button::ButtonFocusChanged(bool is_on)
{
    if (!is_on || !use_focuse_changed_sound) return;
    if (ss_button_focused.upSound->GetState() == ZC_SS__Play) ss_button_focused.upSound->Stop();
    ss_button_focused.upSound->Play();
}

float G_GUI_Button::CalculateButtonWidth(const std::forward_list<G_LangText>& _lang_texts)
{
    static const float add_to_side_borders = 30.f;
    return GetLongestName(_lang_texts) + add_to_side_borders;
}

float G_GUI_Button::GetLongestName(const std::forward_list<G_LangText>& _lang_texts)
{
    float max_length = 0.f;
    for (const G_LangText& lt : _lang_texts)
    {
        float length = ZC__GUI::CalculateWstrWidth(lt.text);
        if (length > max_length) max_length = length; 
    }
    return max_length;
}