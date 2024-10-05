#pragma once

#include <ZC/GUI/ZC_GUI__CheckBox.h>
#include <GUI/Text/G_GUI_TextObjKeeper.h>
#include "G_GUI_Button.h"

class G_GUI_CheckBox : public G_GUI_TextObjKeeper<ZC_GUI__CheckBox>, public G_GUI_Button
{
public:
    G_GUI_CheckBox(bool is_on, ZC_Function<void(bool)>&& _callback, bool use_sound, const ZC_GUI_ColorsCheckBox& _colorsCheckBox, std::forward_list<G_LangText>&& _lang_texts);
};
