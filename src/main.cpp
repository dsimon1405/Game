#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/Textured/G_TParticle.h>
#include <ZC/File/ZC_File.h>

void F(const ZC_Vec3<float>&){}
int main(int argv, char** args)
{
    G_System system;
    G_GameManager gm;
    
    // G_Camera cam_test(ZC_Function<void(const ZC_Vec3<float>&)>{&F}, ZC_Vec3<float>{ 0.f, 0.f, 0.f });
    // cam_test.SetConnectionToEvents(true);
    
    // const int tex_width = 512.f;
    // const int tex_height = 512.f;
    // const int tex_width = 360.f;         //  flame
    // const int tex_height = 360.f;        //  flame
    // const int tex_columns_count = 4;     //  flame
    // const int tex_rows_count = 4;        //  flame
    
    // const int tex_width = 1005.f;        //  bubble
    // const int tex_height = 670.f;       //  bubble
    // const int tex_columns_count = 3;    //  bubble
    // const int tex_rows_count = 2;       //  bubble
    // ZC_Texture::LoadTexture2D(ZC_FSPath(ZC_assetsDirPath).append("Game/textures/bubble.png").string().c_str(), 0, GL_REPEAT, GL_REPEAT);
    // // ZC_Texture::LoadTexture2D(ZC_FSPath(ZC_assetsDirPath).append("Game/textures/flame.png").string().c_str(), 0, GL_REPEAT, GL_REPEAT);
    G_ParticleSystem tp(G_PS_Source
        {
            .particles_count = 1,
            .life_space = G_PS_Source::LS__world,
            .render = G_PS_Source::Render
                {
                    .render_level = ZC_RL_Default,
                    .drawer_level = G_DL_AlphaBlending_ParticleFlame,
                    .tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/textures/bubble.png").string(),
                    .columns_count = 2,
                    .rows_count = 3,
                },
            .spawn_shape = G_PS_Source::SpawnShape
                {
                    .shape = G_PS_Source::SpawnShape::S__Sphere,
                    .val_1 = 0.f,
                    .val_2 = 10.f
                },
            .spawn_mat_model = G_PS_Source::SpawnMatModel
                {
                    .position = { 0.f, 0.f, 5.f },
                },
            .size = G_PS_Source::Size
                {
                    .width = 2.f,
                    .height = 2.f,
                },
            .life_time = G_PS_Source::LifeTime
                {
                    .secs_to_start_max = 3.f,
                    .secs_min = 2.f,
                    .secs_max = 10.f
                },
            .visibility = G_PS_Source::Visibility
                {
                    .appear_secs = 0.8,
                    .disappear_secs = 1.F,
                },
            .move = G_PS_Source::Move
                {
                    .direction_type = G_PS_Source::Move::DT__variable_is_direction,
                    .move_variable = { 1.f, 0.f, 0.f },
                    .speed_power = 1.f,
                    .speed_min = 4.f,
                    .speed_max = 13.f
                },
            .animation = G_PS_Source::Animation
                {
                    .change_tyles_style = G_PS_Source::Animation::CHT__Loop,
                    .tiles_per_second = 7.f,
                    .offset_from = G_PS_Source::Animation::OF__Start,
                    .secs_offset_to_start_animation = 0.f
                }
        });

        // float tiles_per_second = 7.f;
        // // float width = 30.f;      //  flame
        // // float height = 30.f;     //  flame
        // float width = 2.f;         //  buuble
        // float height = 2.f;        //  buuble
        // float secs_to_start_max = 3.f;  //  takes random from 0 to secs_to_start_max

        // float appear_secs = 0.8f;           //  BUBBLE
        // float disappear_secs = 1.f;         //  BUBBLE
        // // float appear_secs = 0.2f;        //  FLAME
        // // float disappear_secs = 0.6f;     //  FLAME

        // float pos_x = 0.f;
        // float pos_y = 0.f;
        // float pos_z = 5.f;
        // float rot_angle = 0.f;
        // float rot_axis_x = 0.f;
        // float rot_axis_y = 0.f;
        // float rot_axis_z = 0.f;

        // float life_secs_min = 2.f;
        // float life_secs_max = 10.f;          //  BUBBLE
        // // float life_secs_max = 3.f;       //  FLAME

        // float circle_radius = 10.f;         //  BUBBLE
        // float particles_count = 1.f;        //  BUBBLE
        // // float circle_radius = 5.f;       //  FLAME
        // // float particles_count = 18.f;    //  FLAME

        // // float move_to_x = 0.f;
        // // float move_to_y = 0.f;
        // // float move_to_z = 6.f;
        // float move_speed_power = 1.f;
        // float move_speed_min_sec = 4.f;
        // float move_speed_max_sec = 13.f;         //  BUBBLE
        // // float move_speed_max_sec = 7.f;      //  FLAME
        // G_PS_Source::LifeSpace life_space = G_PS_Source::LifeSpace::LS__world;

    system.RunMainCycle();
    
    return 0;
}

//  G_Star::Callback_Updater(), G_PlatformDamage::Callback_Updater(), G_Section::GetRandomPlatform(), G_Section::Callback_Updater()

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system