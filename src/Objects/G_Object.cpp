#include "G_Object.h"

#include "Model/G_Models.h"

G_Object::G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO)
    : dsCon(G_Models::GetModel_DSController(modelName, texSetId)),
    upCO(std::move(_upCO))
{
    dsCon.SetUniformsData(ZC_UN_unModel, upCO->GetModelMatrix());
    dsCon.SetUniformsData(ZC_UN_unColor, &unColor);
    dsCon.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
    dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);
}

ZC_Vec3<float> G_Object::VGetPosition_O() noexcept
{
    return ZC_Vec::Vec4_to_Vec3((*(upCO->GetModelMatrix()))[3]);
}