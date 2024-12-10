#include "G_Func.h"

#include <ZC/Tools/Math/ZC_Math.h>

#include <cassert>

ZC_Vec3<float> G_InterpolateColor(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef)
{
    auto lamb_Calc = [growing_coef](float start, float end)
    {
        return start > end ? start - ((start - end) * growing_coef) : start + ((end - start) * growing_coef);
    };
    return { lamb_Calc(start_color[0], end_color[0]), lamb_Calc(start_color[1], end_color[1]), lamb_Calc(start_color[2], end_color[2]) };
}

unsigned int G_InterpolateColor_PackToUInt(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef)
{
    uint res = ZC_Pack_Float_To_UInt_2x10x10x10(G_InterpolateColor(start_color, end_color, growing_coef));
    // assert(end_color != ZC_Vec3<float>() && res != 0u);
    return res;
}