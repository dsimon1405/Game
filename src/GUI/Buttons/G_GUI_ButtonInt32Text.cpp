#include "G_GUI_ButtonInt32Text.h"

#include <ZC/GUI/ZC__GUI.h>

G_GUI_ButtonInt32Text::G_GUI_ButtonInt32Text(int _number, int _number_min, int _number_max, ZC_Function<void(int)>&& _callback_number_changed, bool use_sound,
        const ZC_GUI_ColorsButtonNumber& colorsButtonNumber, std::forward_list<G_LangText>&& _lang_texts)
    : G_GUI_TextObjKeeper<ZC_GUI__ButtonNumberText<int>>(
        ZC_GUI__ButtonNumberText<int>(
            ZC_GUI_ButtonNumber<int>(ZC__GUI::GetFontHeight() * 3, 0.f, _number, _number_min, _number_max, 1, 3, 0, ZC_GUI_TextAlignment::Center,
                std::move(_callback_number_changed),
                use_sound ? ZC_Function<void(bool)>(&G_GUI_Button::ButtonFocusChanged, static_cast<G_GUI_Button*>(this)) : ZC_Function<void(bool)>()),
                ZC_GUI_TextForButton(
                    ZC_GUI_TFB_Indent(ZC__GUI::GetFontHeight() / 2.f, ZC_GUI_TFB_Indent::OutOfButtonLeft),
                    GetCurrentLanguageText(_lang_texts),
                    false,
                    GetLongestWidth(_lang_texts),
                    ZC_GUI_TextAlignment::Left,
                    colorsButtonNumber.color_text)
            ),
        std::move(_lang_texts)),
    G_GUI_Button(use_sound)
{}