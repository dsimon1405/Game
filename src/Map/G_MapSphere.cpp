#include "G_MapSphere.h"

#include <Model/G_Models.h>
#include <System/G_RenderLevel.h>

G_MapSphere::G_MapSphere()
    : dsCon_sphere_map(G_Models::GetModel_DSController(G_MN__SphereMap, 0))
{
    dsCon_sphere_map.SetUniformsData(ZC_UN_unModel, &unModel);
    dsCon_sphere_map.SetUniformsData(ZC_UN_unColor, &unColor);
    dsCon_sphere_map.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
    dsCon_sphere_map.SwitchToDrawLvl(ZC_RL_Default, G_DL_CubeMap);
}

void G_MapSphere::SetScale(float _scale)
{
        //  scale sphere
    scale = _scale;
    unModel = ZC_Mat4<float>(1.f).Rotate(-45.f, {1.f, 0.f, 0.f}).Scale(scale, scale, scale);
}

float G_MapSphere::GetScale() const noexcept
{
    return scale;
}