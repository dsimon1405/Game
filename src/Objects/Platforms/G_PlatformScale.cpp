#include "G_PlatformScale.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_Func.h>

G_PlatformScale::G_PlatformScale(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()))
{}

void G_PlatformScale::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ch_d.scale_state = SS_start;
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformScale::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
    }
}

std::vector<G_GameSound> G_PlatformScale::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(2);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_scale));
    return sounds;
}

void G_PlatformScale::Callback_Updater(float time)
{
    static const float seconds_pause = 1.f;
    static const float seconds_scale = 2.f;
    static const float scale_min = G_Map::scaleXY_other_platforms * 0.2f;
    static const float scale_max = G_Map::scaleXY_other_platforms * 1.f;
    static const float scale_Z = G_Map::scaleZ_platform;
    static const ZC_Vec3<float> scale_color { 1.f, 1.f, 0.f };
    static const uint scale_color_packed = ZC_PackColorFloatToUInt_RGB(scale_color[0], scale_color[1], scale_color[2]);
    static const float deactivate_time = 1.f;

    ch_d.scale_time += time;
    switch (ch_d.scale_state)
    {
    case SS_start: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_down;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
            this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Play);
        }
        else this->unColor = G_InterpolateColor(G_Platform::color_white, scale_color, ch_d.scale_time / seconds_pause);
    } break;
    case SS_scale_down: 
    {
        if (ch_d.scale_time >= seconds_scale)
        {
            ch_d.scale_state = SS_pause_on_down;
            ch_d.scale_time -= seconds_scale;
                //  update model matrix and radius for collision object
            ZC_Vec3<float> scale(scale_min, scale_min, scale_Z);
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            CheckPlatformHabitability(scale_min);
        }
        else
        {
            float scale_coef = scale_max - ((scale_max - scale_min) * (ch_d.scale_time / seconds_scale));
            ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                //  update model matrix and radius for collision object
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
        }
    } break;
    case SS_pause_on_down: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_up;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
            // this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Stop);
            this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Play);
        }
        else 
        {
            float half_pause_time = seconds_pause / 2.f;
            if (ch_d.scale_time < half_pause_time) this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, ch_d.scale_time / half_pause_time);
            else this->unColor = G_InterpolateColor(G_Platform::color_default, scale_color, (ch_d.scale_time - half_pause_time) / half_pause_time);
        }
    } break;
    case SS_scale_up: 
    {
        if (ch_d.scale_time >= seconds_scale)
        {
            ch_d.scale_state = SS_pause_on_up;
            ch_d.scale_time -= seconds_scale;
                //  update model matrix and radius for collision object
            ZC_Vec3<float> scale(scale_max, scale_max, scale_Z);
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            CheckPlatformHabitability(scale_max);
        }
        else
        {
            float scale_coef = scale_min + ((scale_max - scale_min) * (ch_d.scale_time / seconds_scale));
            ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                //  update model matrix and radius for collision object
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
        }
    } break;
    case SS_pause_on_up: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_down;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
            // this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Stop);
            this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Play);
        }
        else 
        {
            float half_pause_time = seconds_pause / 2.f;
            if (ch_d.scale_time < half_pause_time) this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, ch_d.scale_time / half_pause_time);
            else this->unColor = G_InterpolateColor(G_Platform::color_default, scale_color, (ch_d.scale_time - half_pause_time) / half_pause_time);
        }
    } break;
    case SS_end: 
    {
        if (ch_d.scale_time >= deactivate_time)
        {
            ecUpdater.Disconnect();
            ch_d = {};
            this->unColor = 0;
            if (ch_d.deactivate_scale != scale_max)     //  set default scaling
            {
                ZC_Vec3<float> scale(scale_max, scale_max, scale_Z);
                this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            }
        }
        else
        {
            float time_coef = ch_d.scale_time / deactivate_time;
            if (ch_d.deactivate_scale != scale_max)     //  platfrom start deactivate not on max scale, need scale to max
            {
                float scale_coef = scale_min + ((scale_max - scale_min) * time_coef);
                ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                    //  update model matrix and radius for collision object
                this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            }
            this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, time_coef);
        }
    } break;
    }
}

void G_PlatformScale::CheckPlatformHabitability(float scale_val)
{
    for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )   //  check is object still in platforms cylindric radius
    {
        iter = this->IsObjectInPlatformRadiusXY(*iter) ? ++iter : this->objects_on_platform.erase(iter);   //  insecond case object out of cylindric radius of the platform, stop pushing them
    }
    if (this->objects_on_platform.size() == 0)      //  stop activity
    {
        ch_d.scale_state = SS_end;
        ch_d.scale_time = 0.f;
        ch_d.deactivate_scale = scale_val;
        this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Stop);
        this->upSK->SetSoundState(G_SN__platform_scale, ZC_SS__Play);
    }
}