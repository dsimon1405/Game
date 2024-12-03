#include "G_GUI_ButtonText.h"

#include <ZC/GUI/ZC__GUI.h>

G_GUI_ButtonText::G_GUI_ButtonText(const ZC_GUI_Font* pFont, ZC_Function<void(float)>&& _callback_button_up, const ZC_GUI_ColorsButtonText& _colorsButtonText, bool use_sound,
        std::forward_list<G_LangText>&& _lang_texts)
    : G_GUI_TextObjKeeper<ZC_GUI__ClickMouseText>(
        ZC_GUI__ClickMouseText(
            CalculateButtonWidth(pFont, _lang_texts),
            pFont->GetHeight() + 5.f, //  5.f add to top/bottom of the buttons hqight
            ZC_GUI_TextForButton(
                ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent::Center),
                pFont,
                GetCurrentLanguageText(_lang_texts),
                false,
                GetLongestWidth(pFont, _lang_texts),
                ZC_GUI_TextAlignment::Center,
                _colorsButtonText.text_color),
            nullptr,
            std::move(_callback_button_up),
            use_sound ? ZC_Function<void(bool)>(&G_GUI_Button::ButtonFocusChanged, static_cast<G_GUI_Button*>(this)) : ZC_Function<void(bool)>()),
        std::move(_lang_texts)),
    G_GUI_Button(use_sound)
{}