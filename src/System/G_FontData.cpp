#include "G_FontData.h"

#include <ZC/Objects/Text/ZC_Fonts.h>

void G_FontData::LoadFonts()
{
    fd_Arial_30 = ZC_FontData{ .name = ZC_F_Arial, .pixelsHeight = 30 };
    fd_Arial_200 = ZC_FontData{ .name = ZC_F_Arial, .pixelsHeight = 200 };
    fd_Arial_80 = ZC_FontData{ .name = ZC_F_Arial, .pixelsHeight = 80 };

    ZC_FontData fontDatas[] { fd_Arial_30, fd_Arial_200, fd_Arial_80 };
    ZC_Fonts::Load(fontDatas, sizeof(fontDatas) / sizeof(ZC_FontData));
}