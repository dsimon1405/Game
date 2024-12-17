#pragma once

#include <System/G_NewV.h>
#ifndef G_NewV
#include <ZC/Video/OpenGL/Renderer/ZC_DSController.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

    //  like skybox
class G_MapSphere
{
public:
    G_MapSphere();
        //  set scale for sphere map and return eadius of the sphere map
    void SetScale(float _scale);

private:
    ZC_DSController dsCon_sphere_map;
    ZC_Mat4<float> unModel { 1.f };
    uint unColor = 0u;
    float unAlpha = 1.f;
    float scale = 1.f;
};
#endif