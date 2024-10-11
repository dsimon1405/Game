#include "G_PlatformWind.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <System/G_Func.h>
#include <GamePlay/G_Map.h>
#include <ZC/Tools/Math/ZC_Math.h>

G_PlatformWind::G_PlatformWind(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()))
{
    if (!upPS_wind)     //  create wind particles one for all wind platforms
    {
        float wind_width = G_Map::radiusXY_other_platforms * 2.f;
        float wind_length = wind_width * 2.f;
        upPS_wind = new G_PS_Wind(10000, {}, wind_width, 6.f, {}, wind_length);
    }
}

G_PlatformWind::~G_PlatformWind()
{
    if (upPS_wind && pParticles_holder == this)
    {
        upPS_wind->SetDrawState(false);
        pParticles_holder = nullptr;
    }
}

void G_PlatformWind::FreeParticles()
{
    upPS_wind = nullptr;
}

void G_PlatformWind::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformWind::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
        this->upSK->SetVolume(G_SN__platform_wind, sound_wind_start);
        this->upSK->SetSoundState(G_SN__platform_wind, ZC_SS__PlayLoop);
            //  first wind diraction
        ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i),
            { 0.f, 0.f, 1.f }) * G_start_front_pos_v4);
        upPS_wind->SetAlpha(0.f);
    }
}

void G_PlatformWind::VDeactivatePlatform_P()
{
    ch_d.wind_phase = wind_end;      //  stop activity
    ch_d.cur_time = 0.f;
    ch_d.deactivate_color = ZC_UnpackUINTtoFloat_RGB(this->unColor);
    ch_d.deactivate_sound_wind = this->upSK->GetVolume(G_SN__platform_wind);
    ch_d.deactivate_particles_alpha = upPS_wind->GetAlpha();
}

std::vector<G_GameSound> G_PlatformWind::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(2);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_wind));
    return sounds;
}

void G_PlatformWind::Callback_Updater(float time)
{
    static const float seconds_to_start_wind = 2.f;
    static const float seconds_for_one_wind_phase = 1.f;
    static const float seconds_deactivate = 1.f;
    static const float wind_push_power_min = 0.4f;
    static const float wind_push_power_max = 0.8f;
    static const float wind_pus_power_range_min_max = wind_push_power_max - wind_push_power_min;
    
    static const float sound_wind_min = wind_push_power_min / wind_push_power_max;
    static const float sound_wind_max = 1.f;
    static const float sound_wind_range_min_max = sound_wind_max - sound_wind_min;

    static const float particles_speed_min = 20.f;
    static const float particles_speed_max = 50.f;
    static const float particles_speed_range_min_max = particles_speed_max - particles_speed_min;
    static const float particles_alpha_min = 0.6f;
    static const float particles_alpha_max = 1.f;
    static const float particles_alpha_range_min_max = particles_alpha_max - particles_alpha_min;
    static const float particles_distance_to_start_pos = G_Map::radiusXY_other_platforms * 3.f;
    static const float particles_start_Z = G_Map::platforms_all_half_height_Z - 0.5f;
    // gl_PointSize = 3;   //  ZC_SWindow::GLEnablePointSize()

    ch_d.cur_time += time;
    if (!ch_d.wind_started)
    {
        if (ch_d.cur_time >= seconds_to_start_wind)    //  time to die (start wind)
        {
            ch_d.wind_started = true;
            ch_d.cur_time = 0.f;
            ch_d.wind_power = wind_push_power_min;
            upPS_wind->SetAlpha(particles_alpha_min);
        }
        else    //  activate platform
        {        //  start coloring platform to 0.4 (min value) using 2 secs range (while platform activate)
            static const float azure_g_b = wind_push_power_min / wind_push_power_max;

            float time_coef = ch_d.cur_time / seconds_to_start_wind;
            this->unColor = G_InterpolateColor(G_Platform::color_white, { 0.f, azure_g_b, azure_g_b }, time_coef);
            this->upSK->SetVolume(G_SN__platform_wind, sound_wind_min * time_coef);

                //  particles proccess
            static const float seconds_to_start_particles = seconds_to_start_wind / 2.f;
            static const float seconds_to_start_show_particles = seconds_to_start_wind - seconds_to_start_particles / 3.f;
            static const float seconds_start_showing_particles = seconds_to_start_wind - seconds_to_start_show_particles;

            if (ch_d.cur_time >= seconds_to_start_particles)
            {
                if (upPS_wind->IsDrawing())     //  add alpha in particles
                {
                    if (ch_d.cur_time >= seconds_to_start_show_particles)
                    {
                        float seconds_after_start_show_particles = ch_d.cur_time - seconds_to_start_show_particles;
                        upPS_wind->SetAlpha(particles_alpha_min * (seconds_after_start_show_particles / seconds_start_showing_particles));  //  go to alpha top
                    }
                }
                else       //  start particles with alpha 0 earlier then start pushing object, to normalize wind seen
                {
                    upPS_wind->SetDrawState(true);
                    upPS_wind->SetSpeed(particles_speed_min);
                    pParticles_holder = this;
                }
            }
        }
    }
    else
    {
        switch (ch_d.wind_phase)
        {
        case wind_up:
        {
            if (ch_d.cur_time >= seconds_for_one_wind_phase)
            {
                ch_d.cur_time = 0.f;
                ch_d.wind_phase = wind_max;
                ch_d.wind_power = wind_push_power_max;
                this->upSK->SetVolume(G_SN__platform_wind, sound_wind_max);
                upPS_wind->SetAlpha(particles_alpha_max);
                upPS_wind->SetSpeed(particles_speed_max);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_min + (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind, sound_wind_min + (sound_wind_range_min_max * time_coef));
                upPS_wind->SetAlpha(particles_alpha_min + (particles_alpha_range_min_max * time_coef));
                upPS_wind->SetSpeed(particles_speed_min + (particles_speed_range_min_max * time_coef));
            }
        } break;
        case wind_max:
        {
            if (ch_d.cur_time >= seconds_for_one_wind_phase)
            {
                ch_d.cur_time = 0.f;
                ch_d.wind_phase = wind_down;
            }
        } break;
        case wind_down:
        {
            if (ch_d.cur_time >= seconds_for_one_wind_phase)
            {
                ch_d.cur_time = 0.f;
                ch_d.wind_phase = wind_min;
                ch_d.wind_power = wind_push_power_min;
                this->upSK->SetVolume(G_SN__platform_wind, sound_wind_min);
                ch_d.angle_of_new_dir = ZC_Random::GetRandomInt(-ZC_angle_180i, ZC_angle_180i);    //  calculate random angle rotate on in wind_minphase
                upPS_wind->SetAlpha(particles_alpha_min);
                upPS_wind->SetSpeed(particles_speed_min);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_max - (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind, sound_wind_max - (sound_wind_range_min_max * time_coef));
                upPS_wind->SetAlpha(particles_alpha_max - (particles_alpha_range_min_max * time_coef));
                upPS_wind->SetSpeed(particles_speed_max - (particles_speed_range_min_max * time_coef));
            }
        } break;
        case wind_min:
        {
            if (ch_d.cur_time >= seconds_for_one_wind_phase)
            {
                ch_d.cur_time = 0.f;
                ch_d.wind_phase = wind_up;
            }
                //  rotate wind to new direction
            ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).
                Rotate(ch_d.angle_of_new_dir * time / seconds_for_one_wind_phase, { 0.f, 0.f, 1.f }) * ch_d.wind_dir_cur);
        } break;
        case wind_end:      //  deactivate platform
        {
            if (ch_d.cur_time >= seconds_deactivate)
            {
                ch_d = {};    //  activity stoped
                ecUpdater.Disconnect();
                this->unColor = 0;  //  default color
                this->upSK->SetSoundState(G_SN__platform_wind, ZC_SS__Stop);
                upPS_wind->SetAlpha(0.f);
                upPS_wind->SetDrawState(false);
                pParticles_holder = nullptr;
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_deactivate;
                this->unColor = G_InterpolateColor(ch_d.deactivate_color, G_Platform::color_default, time_coef);
                this->upSK->SetVolume(G_SN__platform_wind, ch_d.deactivate_sound_wind - (ch_d.deactivate_sound_wind * time_coef));
                upPS_wind->SetAlpha(ch_d.deactivate_particles_alpha - (ch_d.deactivate_particles_alpha * time_coef));
            }
            return;
        } break;
        }

        float wind_power_coef = ch_d.wind_power / wind_push_power_max;
        this->unColor = ZC_PackColorFloatToUInt_RGB(0.f, wind_power_coef, wind_power_coef);
        
        for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )
        {
            ZC_Vec4<float>& platform_pos = (*(this->upCO->GetModelMatrix()))[3];
            ZC_Vec3<float> obj_pos = (*(iter))->VGetPosition_O();
            float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_pos[0], platform_pos[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
            if (distance < (*(iter))->upCO->GetFigure().radius + this->upCO->GetFigure().radius)
            {
                (*(iter))->VPushObjectInDirection_O(G_PushSet(ch_d.wind_dir_cur, ch_d.wind_power));    //  push object
                ++iter;
            }
            else iter = this->objects_on_platform.erase(iter);   //  object out of cylindric radius of the platform, stop pushing them
        }
    }

        //  platrforms are allways moving, better to update particles diraction and position on each call while drawing
    if (pParticles_holder == this && upPS_wind->IsDrawing())
    {
        const ZC_Vec3<float>& platform_center = this->upCO->GetFigure().center_fact;
        ZC_Vec3<float> platform_center_top(platform_center[0], platform_center[1], particles_start_Z);

        ZC_Vec3<float> particles_start_pos = ZC_Vec::MoveByLength(platform_center_top, ch_d.wind_dir_cur * -1.f, particles_distance_to_start_pos);
        upPS_wind->SetStartPosition(particles_start_pos);
        upPS_wind->SetWindDiraction(platform_center_top - particles_start_pos);
    }
}