#include "G_Object.h"

#include "Model/G_Models.h"

G_Object::G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO, ZC_uptr<G_GameSounds>&& _upSK)
    : dsCon(G_Models::GetModel_DSController(modelName, texSetId)),
    upCO(std::move(_upCO)),
    upSK(std::move(_upSK))
{
    dsCon.SetUniformsData(ZC_UN_unModel, upCO->GetModelMatrix());
    dsCon.SetUniformsData(ZC_UN_unColor, &unColor);
    dsCon.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
    dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);

    if (upSK) upSK->SetPosition(&(upCO->GetFigure().center_fact));
}

void G_Object::SetDefaultState()
{
    if (upSK) upSK->SetDefaultState();
    VSetDefaultState_O();
}

ZC_Vec3<float> G_Object::VGetPosition_O() noexcept
{
    return ZC_Vec::Vec4_to_Vec3((*(upCO->GetModelMatrix()))[3]);
}