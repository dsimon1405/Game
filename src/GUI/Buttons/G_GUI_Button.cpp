#include "G_GUI_Button.h"

#include <ZC/GUI/ZC__GUI.h>

void G_GUI_Button::ButtonFocuseChanged(bool is_on)
{
    
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