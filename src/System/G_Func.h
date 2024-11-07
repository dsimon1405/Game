#pragma once

#include <ZC/Tools/Math/ZC_Vec4.h>

static const ZC_Vec4<float> G_start_front_pos_v4{ 0.f, 1.f, 0.f, 1.f };
static const ZC_Vec3<float> G_start_front_pos_v3{ 0.f, 1.f, 0.f };
static const ZC_Vec3<float> G_start_front_dir_v3{ 0.f, 1.f, 0.f };
static const ZC_Vec3<float> G_world_up{ 0.f, 0.f, 1.f };

    //  Interpolate color from start to end with growing coef
ZC_Vec3<float> G_InterpolateColor(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef);
    
    //  Interpolate color from start to end with growing coef, and pack to uint
unsigned int G_InterpolateColor_PackToUInt(const ZC_Vec3<float>& start_color, const ZC_Vec3<float>& end_color, float growing_coef);