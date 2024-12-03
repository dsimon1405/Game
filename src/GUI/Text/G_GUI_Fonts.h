#pragma once

#include <ZC/GUI/Backend/Text/ZC_GUI_Font.h>

#include <vector>

enum G_GUI_FontsNames
{
    G_GUI_FN__Arial_40,
    G_GUI_FN__Arial_20,
};

struct G_GUI_Fonts
{
public:
    static void LoadFonts();
    static const ZC_GUI_Font* Get(G_GUI_FontsNames fn);
    static float GetFontHeight(G_GUI_FontsNames fn);

private:
    static inline std::vector<ZC_GUI_Font*> fonts;
};