#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DSController.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

class G_CylinderDMG
{
public:
    G_CylinderDMG();

    void SetDrawState(bool need_draw);
    void SetPosition(const ZC_Vec3<float>& pos);
    void SetAlpha(float alpha);
    
private:
    ZC_DSController ds_con;
    ZC_Mat4<float> mat_model;
    float unAlpha = 1.f;
};