#include "G_FontData.h"

#include <ZC/Objects/Text/ZC_Fonts.h>

void G_FontData::LoadFonts()
{
    fd__minus_hp = ZC_FontData{ .name = ZC_F_Arial, .pixelsHeight = 30 };
    fd__level = ZC_FontData{ .name = ZC_F_Arial, .pixelsHeight = 200 };

    ZC_FontData fontDatas[] { fd__minus_hp, fd__level };
    ZC_Fonts::Load(fontDatas, sizeof(fontDatas) / sizeof(ZC_FontData));
}