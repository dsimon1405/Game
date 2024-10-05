#pragma once

#include <GUI/Text/G_LangText.h>

#include <forward_list>

class G_GUI_Button
{
public:
    G_GUI_Button() = default;
    G_GUI_Button(G_GUI_Button&) = delete;
    G_GUI_Button(G_GUI_Button&&) = delete;
    virtual ~G_GUI_Button() = default;

    void ButtonFocuseChanged(bool is_on);

    static float CalculateButtonWidth(const std::forward_list<G_LangText>& _lang_texts);
    static float GetLongestName(const std::forward_list<G_LangText>& _lang_texts);
};