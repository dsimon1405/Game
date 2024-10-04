#include "G_Func.h"

#include <ZC/Tools/Math/ZC_Math.h>

#include <cassert>

unsigned int G_InterpolateColor(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef)
{
    auto lamb_Calc = [growing_coef](float start, float end)
    {
        return start > end ? start - ((start - end) * growing_coef) : start + ((end - start) * growing_coef);
    };
    uint res = ZC_PackColorFloatToUInt_RGB(lamb_Calc(start_color[0], end_color[0]), lamb_Calc(start_color[1], end_color[1]), lamb_Calc(start_color[2], end_color[2]));
    assert(res != 0);
    return res;
}