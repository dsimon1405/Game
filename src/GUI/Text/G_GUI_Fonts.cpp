#include "G_GUI_Fonts.h"

#include <ZC/GUI/ZC__GUI.h>

void G_GUI_Fonts::LoadFonts()
{
    std::vector<ZC_GUI_FontLoadData> load_data
        {
            ZC_GUI_FontLoadData{ ZC_GUI_FontLoadData::ZC_GUI_FontName::Arial, 40, ZC_GUI_FE__Symbols | ZC_GUI_FE__English | ZC_GUI_FE__Russian },
            ZC_GUI_FontLoadData{ ZC_GUI_FontLoadData::ZC_GUI_FontName::Arial, 20, ZC_GUI_FE__Symbols | ZC_GUI_FE__English | ZC_GUI_FE__Russian },
        };
    fonts = ZC__GUI::LoadFonts(load_data, {});
}

const ZC_GUI_Font* G_GUI_Fonts::Get(G_GUI_FontsNames fn)
{
    return fonts[fn];
}

float G_GUI_Fonts::GetFontHeight(G_GUI_FontsNames fn)
{
    return fonts[fn]->GetHeight();
}