#pragma once

#include <ZC/GUI/Button/ZC_GUI__ButtonMouseText.h>
#include <ZC/GUI/Button/ZC_GUI__Click.h>
#include "G_LangText.h"
#include "G_GUI_TextObjKeeper.h"

class G_GUI_ButtonText : public G_GUI_TextObjKeeper<ZC_GUI__Click<ZC_GUI__ButtonMouseText>>
{
public:
    G_GUI_ButtonText(float width, float height, ZC_Function<void(float)>&& _callback_button_up, ZC_Function<void(bool)>&&  _callback_focuse_changed,
        const ZC_GUI_ColorsButtonText& _colorsButtonText, std::forward_list<G_LangText>&& _lang_texts);
};
