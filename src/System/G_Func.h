#pragma once

#include <ZC/Tools/Math/ZC_Vec3.h>

    //  Interpolate color from start to end with growing!!! coef
unsigned int G_InterpolateColor(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef);