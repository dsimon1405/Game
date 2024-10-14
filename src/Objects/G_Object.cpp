#include "G_Object.h"

#include "Model/G_Models.h"

G_Object::G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO, ZC_uptr<G_GameSoundSet>&& _upSK)
    : G_IObject(),
    dsCon(G_Models::GetModel_DSController(modelName, texSetId)),
    upCO(std::move(_upCO)),
    upSK(std::move(_upSK))
{
    dsCon.SetUniformsData(ZC_UN_unModel, upCO->GetModelMatrix());
    dsCon.SetUniformsData(ZC_UN_unColor, &unColor);
    dsCon.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
    dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);

    if (upSK) upSK->SetPosition(&(upCO->GetFigure().center_fact));
}

void G_Object::VSetDefaultState_IO()
{
    VSetDefaultState_O();
    if (upSK) upSK->SetDefaultState();
}

void G_Object::SetSoundSetToDefault()
{
    if (upSK) upSK->SetDefaultState();
}

float G_Object::GetRadius()
{
    return upCO->GetFigure().radius;
}

ZC_Vec3<float> G_Object::VGetPosition_IO() noexcept
{
    return ZC_Vec::Vec4_to_Vec3((*(upCO->GetModelMatrix()))[3]);
}

float G_Object::VGetRadius_IO() const noexcept
{
    return upCO->GetFigure().radius;
}