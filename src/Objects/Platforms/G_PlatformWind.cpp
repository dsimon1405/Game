#include "G_PlatformWind.h"

#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <System/G_Func.h>
#include <Map/G_Map.h>
#include <ZC/Tools/Math/ZC_Math.h>

#ifdef G_NewV
#include <System/G_RenderLevel.h>
#include <ZC/File/ZC_File.h>
G_PlatformWind::G_PlatformWind(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()), G_PT__Win)
{
    if (!upPS)     //  create wind particles one for all wind platforms
    {
        float scale = G_Map::radiusXY_other_platforms + 1.f;
        upPS = new G_ParticleSystem(
            G_PS_Source
                {
                    .particles_count = 150,     //  b
                    .render = G_PS_Source::Render
                        {
                            .render_level = ZC_RL_Default,
                            .drawer_level = G_DL_AlphaBlending_ParticleSystem_WindBubble,
                            .tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/textures/bubble.png").string(),     //  b
                            .columns_count = 3,      //  b
                            .rows_count = 2,     //  b
                        },
                    .spawn_shape = G_PS_Source::SpawnShape
                        {
                            .shape = G_PS_Source::SpawnShape::S__Hemisphere,
                            .fill_to_center = 0.7f
                        },
                    .spawn_mat_model = G_PS_Source::SpawnMatModel
                        {
                            .scale = { scale, scale, scale }
                        },
                    .size = G_PS_Source::Size
                        {
                            .width = 1.8f,    //  b
                            .height = 1.8f,   //  b
                        },
                    .life_time = G_PS_Source::LifeTime
                        {
                            .secs_to_start_max = 3.f,
                            .secs_min = 1.f,
                            .secs_max = 2.f    //  b
                        },
                    .move = G_PS_Source::Move
                        {
                            .direction_type = G_PS_Source::Move::DT__variable_is_direction,
                            .variable = { -1.f, -1.f, 0.f },
                            .speed_power = 1.f,
                            .speed_min = 30.f,
                            .speed_max = 40.f   //  b
                        },
                    .rotate = G_PS_Source::Rotate
                        {
                            .angle_use = G_PS_Source::Rotate::AU__random_between_constants,
                            .angle_1 = -180.f,
                            .angle_2 = 180.f,
                        },
                    .animation = G_PS_Source::Animation
                        {
                            .repaet = G_PS_Source::Animation::R_Single_pass,
                            .tiles_per_second = 20.f,
                            .offset_from = G_PS_Source::Animation::OF__End,
                            .offset_to_start_animation_secs = 0.7f
                        },
                    .color = G_PS_Source::Color
                        {
                            .rgb_use = G_PS_Source::Color::RGBU_Add,
                            .appear_secs = 0.2f,
                            .disappear_secs = 0.2F,
                            .rgba_start = { 0.f, 0.f, 0.f, 0.f },
                            .rgba_end = { 0.f, 0.f, 0.f, 0.f },
                        },
                },
            G_ParticleSystem::Collision 
                {
                    .particle_radius = 0.75f,
                    .set_life_time = G_ParticleSystem::Collision::SLT_Start_animation,
                    .move_speed = -0.95f
                }
            );
    }
}

G_PlatformWind::~G_PlatformWind()
{
    if (upPS && pParticles_holder == this)
    {
        upPS->SetDrawState(false);
        pParticles_holder = nullptr;
    }
}

void G_PlatformWind::FreeParticles()
{
    upPS = nullptr;
}

void G_PlatformWind::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformWind::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
        this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_start);
        this->upSK->SetSoundState(G_SN__platform_wind_wind, ZC_SS__PlayLoop);
        ch_d = {};
            //  first wind direction
        ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i),
            { 0.f, 0.f, 1.f }) * G_start_front_pos_v4);
    }

    bool add_to_collision = true;
    for (PS_Collision& ps_c : ps_collisions)    //  check is object allready added in ps_collision with other pWindPlatform
    {
        if (ps_c.pObj == pObj_add) ps_c.pHolder = this;     //  object allready added, change holder
        add_to_collision = false;
    }
    const ZC_CO_FigureSphere& fig = pObj_add->upCO->GetFigure();
    if (add_to_collision) ps_collisions.emplace_back(PS_Collision{ .pHolder = this, .pObj = pObj_add, .collision_id = upPS->AddCollisionObject(fig.radius, fig.center_fact) });    //  add to collisoin
}

void G_PlatformWind::VDeactivatePlatform_P()
{
    ch_d.wind_phase = wind_end;      //  stop activity
    ch_d.cur_time = 0.f;
    ch_d.deactivate_color = ZC_Unpack_UInt_2x10x10x10_To_Float(this->unColor);
    ch_d.deactivate_sound_wind = this->upSK->GetVolume(G_SN__platform_wind_wind);
    ch_d.deactivate_particles_alpha = upPS->c_ps_src.color.system_alpha;
}

std::vector<G_GameSound> G_PlatformWind::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(3);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_wind_wind));
    sounds.emplace_back(G_GameSound(G_SN__platform_wind_bubble));
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

    static const float particles_speed_min = 0.6f;
    static const float particles_speed_max = 1.f;
    static const float particles_speed_range_min_max = particles_speed_max - particles_speed_min;
    static const float particles_distance_to_start_pos = G_Map::radiusXY_other_platforms * 3.f;
    static const float particles_start_Z = G_Map::platforms_all_half_height_Z - 0.5f;

    ch_d.cur_time += time;
    if (!ch_d.wind_started)
    {
        if (ch_d.cur_time >= seconds_to_start_wind)    //  time to die (start wind)
        {
            ch_d.wind_started = true;
            ch_d.cur_time = 0.f;
            ch_d.wind_power = wind_push_power_min;

            upPS->Set_Color__system_alpha(1.f);
            upPS->SetDrawState(true);
            pParticles_holder = this;
            this->upSK->SetPosition(&(this->upCO->GetFigure().center_fact));

            static const float time_to_start_bubble_sound = 1.f;
            bubble_sound_secs_cur = time_to_start_bubble_sound;    //  restart sound bubble time
        }
        else    //  activate platform
        {        //  start coloring platform to 0.4 (min value) using 2 secs range (while platform activate)
            static const float azure_g_b = wind_push_power_min / wind_push_power_max;

            float time_coef = ch_d.cur_time / seconds_to_start_wind;
            this->unColor = G_InterpolateColor_PackToUInt_2x10x10x10(G_Platform::color_white, { 0.f, azure_g_b, azure_g_b }, time_coef);
            this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min * time_coef);
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
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_max);
                upPS->Set_Move__speed_power(particles_speed_max);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_min + (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min + (sound_wind_range_min_max * time_coef));
                upPS->Set_Move__speed_power(particles_speed_min + (particles_speed_range_min_max * time_coef));
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
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min);
                ch_d.angle_of_new_dir = ZC_Random::GetRandomInt(-ZC_angle_180i, ZC_angle_180i);    //  calculate random angle rotate on in wind_minphase
                upPS->Set_Move__speed_power(particles_speed_min);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_max - (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_max - (sound_wind_range_min_max * time_coef));
                upPS->Set_Move__speed_power(particles_speed_max - (particles_speed_range_min_max * time_coef));
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
                this->upSK->SetSoundState(G_SN__platform_wind_wind, ZC_SS__Stop);
                if (pParticles_holder == this)
                {
                    upPS->SetDrawState(false);
                    pParticles_holder = nullptr;
                    upSK->SetSoundState(G_SN__platform_wind_bubble, ZC_SS__Stop);   //  the owner of the bubble sound is the same as that of the particles, only owner may controll them
                }
                    //  delete object from collisons if they are belong to us
                for (auto iter = ps_collisions.begin(); iter != ps_collisions.end(); )
                {
                    if (iter->pHolder == this)
                    {
                        upPS->DeleteCollisionObject(iter->collision_id);    //  free space in particle system
                        iter = ps_collisions.erase(iter);
                    }
                    else ++iter;
                }
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_deactivate;
                this->unColor = G_InterpolateColor_PackToUInt_2x10x10x10(ch_d.deactivate_color, G_Platform::color_default, time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, ch_d.deactivate_sound_wind - (ch_d.deactivate_sound_wind * time_coef));
                if (pParticles_holder == this) upPS->Set_Color__system_alpha(ch_d.deactivate_particles_alpha - (ch_d.deactivate_particles_alpha * time_coef));
            }
            return;
        } break;
        }

        float wind_power_coef = ch_d.wind_power / wind_push_power_max;
        this->unColor = ZC_Pack_Float_To_UInt_2x10x10x10(0.f, wind_power_coef, wind_power_coef);
        
        for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )
        {
            ZC_Vec4<float>& platform_pos = (*(this->upCO->GetModelMatrix()))[3];
            ZC_Vec3<float> obj_pos = (*(iter))->VGetPosition_IO();
            float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_pos[0], platform_pos[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
            if (distance < (*(iter))->upCO->GetFigure().radius + this->upCO->GetFigure().radius)
            {
                (*(iter))->VPushObjectInDirection_IO(G_PushSet(ch_d.wind_dir_cur, ch_d.wind_power));    //  push object
                ++iter;
            }
            else iter = this->EraseObjectFromPlatform(iter);   //  object out of cylindric radius of the platform, stop pushing them
        }
    }

        //  platrforms are allways moving, better to update particles direction and position on each call while drawing
    if (pParticles_holder == this && upPS->IsDrawing())
    {
        const ZC_Vec3<float>& platform_center = this->upCO->GetFigure().center_fact;
        ZC_Vec3<float> platform_center_top(platform_center[0], platform_center[1], particles_start_Z);
            //  particles pos and dir
        ZC_Vec3<float> particles_start_pos = ZC_Vec::MoveByLength(platform_center_top, ch_d.wind_dir_cur * -1.f, particles_distance_to_start_pos);
        upPS->Set_SpawnMatModel__translation(particles_start_pos);
        upPS->Set_Move__move_variable(platform_center_top - particles_start_pos);
            //  particles collision
        bool object_on_platform = false;
        for (PS_Collision& ps_c : ps_collisions)    //  update objects pos in collisions
        {
            upPS->SetCollisionObject_world_pos(ps_c.collision_id, ps_c.pObj->upCO->GetFigure().center_fact);
            if (!object_on_platform) object_on_platform = IsObjectInPlatformRadiusXY(ps_c.pObj);    //  check if there is at least one object on the platform
        }
            //  particle sound.
            //  The particle (bubble) data is calculated on the GPU, so some trickery needs to be used to get a near-real bubble sound effect after a collision.
            //  At start sets bubble_sound_secs_cur one second, that's enough for first appeared bubbles to reach an object. Decrease the start time until it reaches 0. 
            //  Then, to get some chaos into the bubble sound effect, take a random value between 0.01 and 0.3 seconds for each subsequent sound trigger. If there's no object on the platform sets unreachable 1000 seconds to next effect.
        static float restart_bubble_sound_secs_min = 1.f;
        static float restart_bubble_sound_secs_max = 30.f;
        bubble_sound_secs_cur -= time;
        if (bubble_sound_secs_cur <= 0.f)
        {
            this->upSK->SetSoundState(G_SN__platform_wind_bubble, ZC_SS__Stop);
            this->upSK->SetSoundState(G_SN__platform_wind_bubble, ZC_SS__Play);
            bubble_sound_secs_cur = object_on_platform ? ZC_Random::GetRandomFloat_x_100(restart_bubble_sound_secs_min, restart_bubble_sound_secs_max) / 100.f : 1000.f;    //  if there is no object on the platform, set to 1000 seconds to avoid bubble sounds
        }
    }
}

#else

G_PlatformWind::G_PlatformWind(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()), G_PT__Win)
{
    if (!upPS_wind)     //  create wind particles one for all wind platforms
    {
        float wind_width = G_Map::radiusXY_other_platforms * 2.f;
        float wind_length = wind_width * 3.f;
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

void G_PlatformWind::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformWind::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
        this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_start);
        this->upSK->SetSoundState(G_SN__platform_wind_wind, ZC_SS__PlayLoop);
        ch_d = {};
            //  first wind direction
        ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i),
            { 0.f, 0.f, 1.f }) * G_start_front_pos_v4);
        upPS_wind->SetAlpha(0.f);
    }
}

void G_PlatformWind::VDeactivatePlatform_P()
{
    ch_d.wind_phase = wind_end;      //  stop activity
    ch_d.cur_time = 0.f;
    ch_d.deactivate_color = ZC_Unpack_UInt_2x10x10x10_To_Float(this->unColor);
    ch_d.deactivate_sound_wind = this->upSK->GetVolume(G_SN__platform_wind_wind);
    ch_d.deactivate_particles_alpha = upPS_wind->GetAlpha();
}

std::vector<G_GameSound> G_PlatformWind::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(2);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_wind_wind));
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

    static const float particles_speed_min = 40.f;
    static const float particles_speed_max = 60.f;
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
            this->unColor = G_InterpolateColor_PackToUInt_2x10x10x10(G_Platform::color_white, { 0.f, azure_g_b, azure_g_b }, time_coef);
            this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min * time_coef);

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
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_max);
                upPS_wind->SetAlpha(particles_alpha_max);
                upPS_wind->SetSpeed(particles_speed_max);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_min + (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min + (sound_wind_range_min_max * time_coef));
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
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_min);
                ch_d.angle_of_new_dir = ZC_Random::GetRandomInt(-ZC_angle_180i, ZC_angle_180i);    //  calculate random angle rotate on in wind_minphase
                upPS_wind->SetAlpha(particles_alpha_min);
                upPS_wind->SetSpeed(particles_speed_min);
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_for_one_wind_phase;
                ch_d.wind_power = wind_push_power_max - (wind_pus_power_range_min_max * time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, sound_wind_max - (sound_wind_range_min_max * time_coef));
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
                this->upSK->SetSoundState(G_SN__platform_wind_wind, ZC_SS__Stop);
                if (pParticles_holder == this)
                {
                    upPS_wind->SetAlpha(0.f);
                    upPS_wind->SetDrawState(false);
                    pParticles_holder = nullptr;
                }
            }
            else
            {
                float time_coef = ch_d.cur_time / seconds_deactivate;
                this->unColor = G_InterpolateColor_PackToUInt_2x10x10x10(ch_d.deactivate_color, G_Platform::color_default, time_coef);
                this->upSK->SetVolume(G_SN__platform_wind_wind, ch_d.deactivate_sound_wind - (ch_d.deactivate_sound_wind * time_coef));
                upPS_wind->SetAlpha(ch_d.deactivate_particles_alpha - (ch_d.deactivate_particles_alpha * time_coef));
            }
            return;
        } break;
        }

        float wind_power_coef = ch_d.wind_power / wind_push_power_max;
        this->unColor = ZC_Pack_Float_To_UInt_2x10x10x10(0.f, wind_power_coef, wind_power_coef);
        
        for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )
        {
            ZC_Vec4<float>& platform_pos = (*(this->upCO->GetModelMatrix()))[3];
            ZC_Vec3<float> obj_pos = (*(iter))->VGetPosition_IO();
            float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_pos[0], platform_pos[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
            if (distance < (*(iter))->upCO->GetFigure().radius + this->upCO->GetFigure().radius)
            {
                (*(iter))->VPushObjectInDirection_IO(G_PushSet(ch_d.wind_dir_cur, ch_d.wind_power));    //  push object
                ++iter;
            }
            else iter = this->EraseObjectFromPlatform(iter);   //  object out of cylindric radius of the platform, stop pushing them
        }
    }

        //  platrforms are allways moving, better to update particles direction and position on each call while drawing
    if (pParticles_holder == this && upPS_wind->IsDrawing())
    {
        const ZC_Vec3<float>& platform_center = this->upCO->GetFigure().center_fact;
        ZC_Vec3<float> platform_center_top(platform_center[0], platform_center[1], particles_start_Z);

        ZC_Vec3<float> particles_start_pos = ZC_Vec::MoveByLength(platform_center_top, ch_d.wind_dir_cur * -1.f, particles_distance_to_start_pos);
        upPS_wind->SetPosition(particles_start_pos);
        upPS_wind->SetWinddirection(platform_center_top - particles_start_pos);
    }
}
#endif