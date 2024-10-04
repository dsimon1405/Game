#pragma once

#include <ZC/Tools/Math/ZC_Vec3.h>

struct G_PushSet
{
    ZC_Vec3<float> dir;
    float power = 0.f;

    G_PushSet(const ZC_Vec3<float>& _dir, float _power);
};