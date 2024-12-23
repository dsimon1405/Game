#include "G_PlatformDisappear.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_RenderLevel.h>
#include <System/G_Func.h>
#include <Map/G_Map.h>

G_PlatformDisappear::G_PlatformDisappear(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()), G_PT__Disapear)
{}

void G_PlatformDisappear::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1ull)
    {
        ch_d.disapear_state = DS_start;
        ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformDisappear::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
    }
}

G_LightSet G_PlatformDisappear::GetLightSet_P()
{
    assert(!objects_on_platform.empty());
    if (unAlpha == 0u) return {};   //  platform wanished, no need to calculate pos and color
        //  when object at the edge of the platform, need to make effect there is no light farther. So dive radous on some value, I found 9.2F.
    static const float light_platform_radius = G_Map::radiusXY_other_platforms - (G_Map::radiusXY_other_platforms / platform_light_radius_divisor);
        //  the light power depends on the visibility of the disappearing platform
    return G_LightSet{ .pos = CalculateLightPos(light_platform_radius), .color = ZC_Pack_Float_To_UInt_2x10x10x10(ch_d.cur_color * this->unAlpha) };
}

std::vector<G_GameSound> G_PlatformDisappear::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(2ull);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_disapear));
    return sounds;
}

void G_PlatformDisappear::Callback_Updater(float time)
{
    static const float seconds_phase = 1.f;
    static const float seconds_half_phase = seconds_phase / 2.f;
    static const float disappear_phase1_alpha = 0.5f;
    static const float disappear_alpha_max = 1.f;
    static const ZC_Vec3<float> disappear_color { 0.f, 0.f, 1.f };
    static const uint disappear_color_packed = ZC_Pack_Float_To_UInt_2x10x10x10(disappear_color);

    ch_d.time += time;
    switch (ch_d.disapear_state)
    {
    case DS_start:
    {
        if (ch_d.time >= seconds_phase)
        {
            ch_d.time = 0.f;
            ch_d.disapear_state = DS_phase1_disapear;
            ch_d.cur_color = disappear_color;
            this->unColor = disappear_color_packed;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_PlatformDisappear);
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Play);
        }
        else
        {
            ch_d.cur_color = G_InterpolateColor(G_Platform::color_white, disappear_color, ch_d.time / seconds_phase);
            this->unColor = ZC_Pack_Float_To_UInt_2x10x10x10(ch_d.cur_color);
        }
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
            this->unAlpha = disappear_alpha_max - (disappear_phase1_alpha * time_coef);
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
            this->unAlpha = disappear_phase1_alpha + (disappear_phase1_alpha * time_coef);
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
            this->unAlpha = disappear_alpha_max - (disappear_phase1_alpha * time_coef);
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
            this->unColor = 0u;
            this->unAlpha = 1.f;
            this->dsCon.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);
            ch_d = {};
            ecUpdater.Disconnect();
            this->upSK->SetSoundState(G_SN__platform_disapear, ZC_SS__Stop);
        }
        else
        {
            float time_coef = ch_d.time / seconds_apear;
            this->unAlpha = disappear_phase1_alpha + (disappear_phase1_alpha * time_coef);
            ch_d.cur_color = G_InterpolateColor(disappear_color, G_Platform::color_default, time_coef);
            this->unColor = ZC_Pack_Float_To_UInt_2x10x10x10(ch_d.cur_color);
        }
    } break;
    }
}

void G_PlatformDisappear::VSetDrawState(bool need_draw)
{
    if (!ecUpdater.IsConnected()) this->dsCon.SwitchToDrawLvl(ZC_RL_Default, need_draw ? ZC_DL_Drawing : ZC_DL_None);   //  change state only if platform is not using now
}