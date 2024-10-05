#include "G_GUI_CheckBox.h"

G_GUI_CheckBox::G_GUI_CheckBox(bool is_on, ZC_Function<void(bool)>&& _callback, bool use_sound, const ZC_GUI_ColorsCheckBox& _colorsCheckBox,
        std::forward_list<G_LangText>&& _lang_texts)
    : G_GUI_TextObjKeeper<ZC_GUI__CheckBox>(
        ZC_GUI__CheckBox(ZC_GUI_TFB_Indent_Location::OutOfButtonRight, this->GetCurrentLanguageText(_lang_texts),
            is_on, std::move(_callback), use_sound ? ZC_Function<void(bool)>(&G_GUI_Button::ButtonFocuseChanged, static_cast<G_GUI_Button*>(this)) : ZC_Function<void(bool)>(),
            _colorsCheckBox, false, GetLongestName(_lang_texts), ZC_GUI_TextAlignment::Left),
        std::move(_lang_texts))
{}