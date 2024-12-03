#pragma once

#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <GUI/Text/G_GUI_TextObjKeeper.h>
#include "G_GUI_Button.h"

class G_GUI_ButtonInt32Text : public G_GUI_TextObjKeeper<ZC_GUI__ButtonNumberText<int>>, public G_GUI_Button
{
public:
    G_GUI_ButtonInt32Text(const ZC_GUI_Font* pFont, int _number, int _number_min, int _number_max, ZC_Function<void(int)>&& _callback_number_changed, bool use_sound,
        const ZC_GUI_ColorsButtonNumber& colorsButtonNumber, std::forward_list<G_LangText>&& _lang_texts);
};