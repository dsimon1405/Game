#pragma once

#include <ZC/Objects/Text/ZC_Font.h>

    //  load font for ZC_Text classes, not ZC_GUI
struct G_FontData
{
    static inline ZC_FontData fd_Arial_30;
    static inline ZC_FontData fd_Arial_200;
    static inline ZC_FontData fd_Arial_80;

    static void LoadFonts();
};