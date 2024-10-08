#include "G_PlatformDisapear.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_RenderLevel.h>
#include <System/G_Func.h>

G_PlatformDisapear::G_PlatformDisapear(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, nullptr)
{}

void G_PlatformDisapear::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ch_d.disapear_state = DS_start;
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformDisapear::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
    }
}

void G_PlatformDisapear::Callback_Updater(float time)
{
    ch_d.time += time;
    switch (ch_d.disapear_state)
    {
    case DS_start:
    {
        if (ch_d.time >= seconds_phase)
        {
            ch_d.time -= seconds_phase;
            ch_d.disapear_state = DS_phase1_disapear;
            this->unColor = disapear_color_packed;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending);
        }
        else this->unColor = G_InterpolateColor(G_Platform::color_white, disapear_color, ch_d.time / seconds_phase);
    } break;
    case DS_phase1_disapear:
    {
        if (ch_d.time >= seconds_half_phase)
        {
            ch_d.time -= seconds_half_phase;
            ch_d.disapear_state = DS_phase1_apear;
        }
        else
        {
            float time_coef = ch_d.time / seconds_half_phase;
            this->unAlpha = disapear_alpha_max - (disapear_phase1_alpha * time_coef);
        }
    } break;
    case DS_phase1_apear:
    {
        if (ch_d.time >= seconds_half_phase)
        {
            ch_d.time -= seconds_half_phase;
            ch_d.disapear_state = DS_phase2_disapear;
        }
        else
        {
            float time_coef = ch_d.time / seconds_half_phase;
            this->unAlpha = disapear_phase1_alpha + (disapear_phase1_alpha * time_coef);
        }
    } break;
    case DS_phase2_disapear:
    {
        if (ch_d.time >= seconds_half_phase)
        {
            ch_d.time = 0.f;
            ch_d.disapear_state = DS_phase2_wait;
            this->unAlpha = 0.f;
            ch_d.temp_mat_model = *(this->upCO->GetModelMatrix());  //  safe cur model matrix
            this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(0.f, 0.f, 1000.f));     //  set default model matrix to drop objects from platform
        }
        else
        {
            float time_coef = ch_d.time / seconds_half_phase;
            this->unAlpha = disapear_alpha_max - (disapear_phase1_alpha * time_coef);
        }
    } break;
    case DS_phase2_wait:
    {
        if (ch_d.time >= 3.f)
        {
            ch_d.time = 0.f;
            ch_d.disapear_state = DS_phase2_apear;
            this->upCO->UpdateModelMatrix(ch_d.temp_mat_model);
        }
    } break;
    case DS_phase2_apear:
    {
        if (ch_d.time >= seconds_phase)
        {
            this->unColor = 0;
            this->unAlpha = 1.f;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);
            ch_d = {};
            ecUpdater.Disconnect();
        }
        else
        {
            float time_coef = ch_d.time / seconds_phase;
            this->unAlpha = disapear_phase1_alpha + (disapear_phase1_alpha * time_coef);
            this->unColor = G_InterpolateColor(disapear_color, G_Platform::color_default, time_coef);
        }
    } break;
    }
}