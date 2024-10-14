#include "G_PlatformDisapear.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_RenderLevel.h>
#include <System/G_Func.h>

G_PlatformDisapear::G_PlatformDisapear(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()))
{}

void G_PlatformDisapear::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ch_d.disapear_state = DS_start;
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformDisapear::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
    }
}

std::vector<G_GameSound> G_PlatformDisapear::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(2);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_disapear));
    return sounds;
}

void G_PlatformDisapear::Callback_Updater(float time)
{
    static const float seconds_phase = 1.f;
    static const float seconds_half_phase = seconds_phase / 2.f;
    static const float disapear_phase1_alpha = 0.5f;
    static const float disapear_alpha_max = 1.f;
    static const ZC_Vec3<float> disapear_color { 0.f, 0.f, 1.f };
    static const uint disapear_color_packed = ZC_PackColorFloatToUInt_RGB(disapear_color[0], disapear_color[1], disapear_color[2]);

    ch_d.time += time;
    switch (ch_d.disapear_state)
    {
    case DS_start:
    {
        if (ch_d.time >= seconds_phase)
        {
            ch_d.time = 0.f;
            ch_d.disapear_state = DS_phase1_disapear;
            this->unColor = disapear_color_packed;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_PlatformDisapear);
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Play);
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
            ch_d.temp_translate = this->platf_trans.translate;
            this->platf_trans.Update_translate({ 0.f, 0.f, 1000.f });
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Pause);
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
            this->platf_trans.Update_translate(ch_d.temp_translate);
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Play);
        }
    } break;
    case DS_phase2_apear:
    {
        static float seconds_apear = 1.5f;
        if (ch_d.time >= seconds_apear)
        {
            this->unColor = 0;
            this->unAlpha = 1.f;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);
            ch_d = {};
            ecUpdater.Disconnect();
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Stop);
        }
        else
        {
            float time_coef = ch_d.time / seconds_apear;
            this->unAlpha = disapear_phase1_alpha + (disapear_phase1_alpha * time_coef);
            this->unColor = G_InterpolateColor(disapear_color, G_Platform::color_default, time_coef);
        }
    } break;
    }
}