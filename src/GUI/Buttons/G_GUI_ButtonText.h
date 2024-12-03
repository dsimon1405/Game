#pragma once

#include <ZC/GUI/Button/ZC_GUI__Click.h>
#include <GUI/Text/G_GUI_TextObjKeeper.h>
#include "G_GUI_Button.h"

class G_GUI_ButtonText : public G_GUI_TextObjKeeper<ZC_GUI__ClickMouseText>, public G_GUI_Button
{
public:
    G_GUI_ButtonText(const ZC_GUI_Font* pFont, ZC_Function<void(float)>&& _callback_button_up, const ZC_GUI_ColorsButtonText& _colorsButtonText, bool use_sound,
        std::forward_list<G_LangText>&& _lang_texts);
};
