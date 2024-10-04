#pragma once

#include <ZC/Objects/Text/ZC_Font.h>

    //  load font for ZC_Text classes, not ZC_GUI
struct G_FontData
{
    static inline ZC_FontData fd__minus_hp;
    static inline ZC_FontData fd__level;

    static void LoadFonts();
};