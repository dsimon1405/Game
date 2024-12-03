#pragma once

#include <ZC/GUI/ZC_GUI__Text.h>
#include "G_GUI_TextObjKeeper.h"

class G_GUI_Text : public G_GUI_TextObjKeeper<ZC_GUI__Text>
{
public:
    G_GUI_Text(const ZC_GUI_Font* pFont, ZC_GUI_TextAlignment in_texture_alignment, unsigned int color, std::forward_list<G_LangText>&& _lang_texts);
};