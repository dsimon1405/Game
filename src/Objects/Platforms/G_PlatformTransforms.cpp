#include "G_PlatformTransforms.h"

#include <Model/G_Models.h>

ZC_Mat4<float> G_PlatformTransforms::CalculateModelMatrix() const
{
    ZC_Mat4<float> model(1.f);
    if (rotate_angle_external_Z != 0.f) model.Rotate(rotate_angle_external_Z, { 0.f, 0.f, 1.f });
    model.Translate(translate);
    if (rotate_angle_internal_Z != 0.f) model.Rotate(rotate_angle_internal_Z, { 0.f, 0.f, 1.f });
    model.Scale(scale);
    return model;
}

void G_PlatformTransforms::Update_rotate_angle_external_Z(float angle)
{
    rotate_angle_external_Z = angle > ZC_angle_360f ? angle - ZC_angle_360f : angle < - ZC_angle_360f ? angle + ZC_angle_360f : angle;
    pCO->UpdateModelMatrix(CalculateModelMatrix());
}

void G_PlatformTransforms::Update_translate(const ZC_Vec3<float>& _translate)
{
    translate = _translate;
    pCO->UpdateModelMatrix(CalculateModelMatrix());
}

void G_PlatformTransforms::Update_rotate_angle_internal_Z(float angle)
{
    rotate_angle_internal_Z = angle > ZC_angle_360f ? angle - ZC_angle_360f : angle < - ZC_angle_360f ? angle + ZC_angle_360f : angle;
    pCO->UpdateModelMatrix(CalculateModelMatrix());
}

void G_PlatformTransforms::Update_scale(const ZC_Vec3<float>& _scale, G_ModelName model_name)
{
    scale = _scale;
    pCO->UpdateModelMatrix(CalculateModelMatrix());
    pCO->UpdateRadius(G_Models::CalculateRadius(model_name, scale));
}

void G_PlatformTransforms::SwitchMoselMatrices(G_PlatformTransforms& rPT)
{
    G_PlatformTransforms pt_temp = rPT;
        //  change his data
    rPT = *this;
    rPT.pCO = pt_temp.pCO;  //  must stand his own
    rPT.pCO->UpdateModelMatrix(rPT.CalculateModelMatrix());     //  update update model matrix

        //  change our data
    ZC_CollisionObject* pCO_this = pCO;
    *this = pt_temp;
    pCO = pCO_this;
    pCO->UpdateModelMatrix(CalculateModelMatrix());
}