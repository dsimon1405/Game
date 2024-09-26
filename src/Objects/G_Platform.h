#pragma once

#include "G_Object.h"

struct G_Platform : public G_Object
{
    G_Platform(const ZC_Mat4<float>& model, const ZC_Vec3<float>& scale);

    G_Object_Type GetType() const override;

    // float GetRadius()
    // {
    //     ZC_Mat4<float> model(1.f);
    //     auto vec_4 = model.Scale(scale_platform) * ZC_Vec4<float>(-1.f, -1.f, -1.f, 1.f);
    //     return ZC_Vec::Length<float>({ vec_4[0], vec_4[1], vec_4[2] });
    // }
};