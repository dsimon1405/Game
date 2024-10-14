#include "G_GUI_Text.h"

G_GUI_Text::G_GUI_Text(ZC_GUI_TextAlignment in_texture_alignment, unsigned int color, std::forward_list<G_LangText>&& _lang_texts)
    : G_GUI_TextObjKeeper<ZC_GUI__Text>(ZC_GUI__Text(this->GetCurrentLanguageText(_lang_texts), false, this->GetLongestWidth(_lang_texts),
        in_texture_alignment, color == 0 ? ZC_GUI_Colors::window_text : color), std::move(_lang_texts))
{}