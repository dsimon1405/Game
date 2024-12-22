#include "G_CylinderDMG.h"

#include <Model/G_Models.h>
#include <Model/G_ModelName.h>
#include <System/G_RenderLevel.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Video/OpenGL/Shader/ZC_ShProgs.h>
#include <Map/G_Map.h>

G_CylinderDMG::G_CylinderDMG()
    : ds_con(G_Models::GetModel_DSController(G_MN__Platform_cylinder_black))
{
    ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ShPN_Game_CylinderDMG);
        //  change shader prog
    ds_con.pShProg = &(pShPIS->shProg);
        //  take uniform for new shaser prog
    ds_con.personalData.clear();
    ds_con.personalData.emplace_front(ZC_RSPDUniforms::Make(pShPIS->uniforms.GetCopy()));

    ds_con.SetUniformsData(ZC_UN_unModel, &mat_model);
    ds_con.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
}

void G_CylinderDMG::SetDrawState(bool need_draw)
{
    ds_con.SwitchToDrawLvl(ZC_RL_Default, need_draw ? G_DL_AlphaBlending_CylinderDMG : ZC_DL_None);
}

void G_CylinderDMG::SetPosition(const ZC_Vec3<float>& pos)
{
    static const float scale_z = 25.f;
    static const float scale_zy = G_Map::scaleXY_other_platforms - 0.01f;

    mat_model = ZC_Mat4<float>(1.f).Translate(pos).Scale(scale_zy, scale_zy, G_Map::scaleZ_platform * scale_z);
}

void G_CylinderDMG::SetAlpha(float alpha)
{
    assert(alpha >= 0.f && alpha <= 1.f);
    unAlpha = alpha;
}
// void G_CylinderDMG::Callback_Updater(float time)
// {
//     static const float max_secs = 1.f;
    
//     cur_secs -= time;
//     if (cur_secs >= max_secs)
//     {
//         ds_con.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_None);
//         ec_updater.Disconnect();
//     }
//     else
//     {
//         static const float max_alpha = 1.f;

//         unAlpha = max_alpha * (1.f - (cur_secs / max_secs));
//     }
// }