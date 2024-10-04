#include "G_GUI_ButtonText.h"

G_GUI_ButtonText::G_GUI_ButtonText(float width, float height, ZC_Function<void(float)>&& _callback_button_up, ZC_Function<void(bool)>&&  _callback_focuse_changed,
        const ZC_GUI_ColorsButtonText& _colorsButtonText, std::forward_list<G_LangText>&& _lang_texts)
    : G_GUI_TextObjKeeper<ZC_GUI__Click<ZC_GUI__ButtonMouseText>>(ZC_GUI__Click<ZC_GUI__ButtonMouseText>(ZC_GUI__ButtonMouseText(width, height, ZC_GUI_BF__None,
        ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent::Center), this->GetCurrentLanguageText(_lang_texts), false, this->GetLongestWidth(_lang_texts),
        ZC_GUI_TextAlignment::Center, _colorsButtonText.text_color), _colorsButtonText.colorsButton), nullptr, std::move(_callback_button_up),
        std::move(_callback_focuse_changed)), std::move(_lang_texts))
{}