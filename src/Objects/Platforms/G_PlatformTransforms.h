#pragma once

#include <Model/G_ModelName.h>
#include <ZC/Tools/Math/ZC_Mat4.h>
#include <ZC/Collision/ZC_CollisionObject.h>

struct G_PlatformTransforms
{
    float rotate_angle_external_Z = 0.f;
    ZC_Vec3<float> translate;
    float rotate_angle_internal_Z = 0.f;
    ZC_Vec3<float> scale;

    ZC_CollisionObject* pCO = nullptr;

    ZC_Mat4<float> CalculateModelMatrix() const;
        //  set new angle, recalculate model matrix and call pCO->UpdateModelMatrix()
    void Update_rotate_angle_external_Z(float angle);
        //  set new translate, recalculate model matrix and call pCO->UpdateModelMatrix()
    void Update_translate(const ZC_Vec3<float>& _translate);
        //  set new angle, recalculate model matrix and call pCO->UpdateModelMatrix()
    void Update_rotate_angle_internal_Z(float angle);
        //  set new scale, recalculate model matrix and call pCO->UpdateModelMatrix() and upCO->UpdateRadius()
    void Update_scale(const ZC_Vec3<float>& _scale, G_ModelName model_name);
        //  switch platfrom matrices but stand previous pointers to Collision object (uses in G_Platform::SwitchWithWinPlatform)
    void SwitchMoselMatrices(G_PlatformTransforms& rPT);
};