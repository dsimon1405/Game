#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/New/G_PS_Setup.h>
#include <ZC/File/ZC_File.h>
#include <GUI/Text/G_GUI_Fonts.h>
#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <System/G_RenderLevel.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/GUI/ZC_GUI__Switch.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/GUI/ZC__GUI.h>
#include <GUI/Text/G_GUI_Fonts.h>
#include <ZC/ZC__System.h>
#include <ZC/Events/ZC_Events.h>
void Life_space(ui_zc){}
void Spawn_shape__fill_to_center(uchar){}
ZC_EC ec;
void SYKA_ZAEBAL(float){
    G_GameManager::pGM->gui.w_main_menu.StartNewGame(0.f);
    ec.Disconnect();}

void F(const ZC_Vec3<float>&){}

G_ParticleSystem* pPS = nullptr;
void C(ZC_ButtonID, float)
{
    if (pPS) pPS->SetDrawState(!(pPS->IsDrawing()));
}
#include <ZC/Audio/ZC_Sounds.h>
int main(int argv, char** args)
{
    G_System system;
    G_GameManager gm;
    
    ec = ZC__Updater::Connect({SYKA_ZAEBAL}, 0);
    ZC_Events::ConnectButtonUp(ZC_ButtonID::K_E, { &C });
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

    // G_ParticleSystem ps(
    //     G_PS_Source
    //         {
    //             .particles_count = 150,     //  b
    //             // .particles_count = 10,     //  f
    //             .render = G_PS_Source::Render
    //                 {
    //                     .render_level = ZC_RL_Default,
    //                     .drawer_level = G_DL_AlphaBlending_ParticleSystem_Wind,
    //                     .tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/textures/bubble.png").string(),     //  b
    //                     .columns_count = 3,      //  b
    //                     .rows_count = 2,     //  b
    //                     // .tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/textures/flame.png").string(),      //  f
    //                     // .columns_count = 4,      //  f
    //                     // .rows_count = 4,     //  f
    //                 },
    //             .spawn_shape = G_PS_Source::SpawnShape
    //                 {
    //                     .shape = G_PS_Source::SpawnShape::S__Hemisphere,
    //                     .fill_to_center = 0.7f
    //                 },
    //             .spawn_mat_model = G_PS_Source::SpawnMatModel
    //                 {
    //                     .translate = { 41.f, 0.8f, 1.f },
    //                     .scale = { 8.f, 8.f, 8.f }
    //                 },
    //             .size = G_PS_Source::Size
    //                 {
    //                     .width = 1.8f,    //  b
    //                     .height = 1.8f,   //  b
    //                     // .width = 30.f,      //  f
    //                     // .height = 30.f,     //  f
    //                 },
    //             .life_time = G_PS_Source::LifeTime
    //                 {
    //                     .secs_to_start_max = 3.f,
    //                     .secs_min = 1.5f,
    //                     .secs_max = 2.f    //  b
    //                     // .secs_max = 3.f    //  f
    //                 },
    //             .move = G_PS_Source::Move
    //                 {
    //                     .direction_type = G_PS_Source::Move::DT__variable_is_direction,
    //                     .variable = { -1.f, -1.f, 0.f },
    //                     .speed_power = 1.f,
    //                     .speed_min = 40.f,
    //                     .speed_max = 40.f   //  b
    //                     // .speed_max = 7.f   //  f
    //                 },
    //             .rotate = G_PS_Source::Rotate
    //                 {
    //                     .angle_use = G_PS_Source::Rotate::AU__random_between_constants,
    //                     .angle_1 = -180.f,
    //                     .angle_2 = 180.f,
    //                 },
    //             .animation = G_PS_Source::Animation
    //                 {
    //                     .repaet = G_PS_Source::Animation::R_Single_pass,
    //                     .tiles_per_second = 20.f,
    //                     .offset_from = G_PS_Source::Animation::OF__End,
    //                     .offset_to_start_animation_secs = 0.7f
    //                 },
    //             .color = G_PS_Source::Color
    //                 {
    //                     .rgb_use = G_PS_Source::Color::RGBU_Add,
    //                     .appear_secs = 0.2f,
    //                     .disappear_secs = 0.2F,
    //                     .rgba_start = { 0.f, 0.f, 0.f, 0.f },
    //                     .rgba_end = { 0.f, 0.f, 0.f, 0.f },
    //                     // .color_appear_secs = 0.2,     //  f
    //                     // .color_disappear_secs = 0.6F,     //  f
    //                 },
    //         },
    //     G_ParticleSystem::Collision 
    //         {
    //             .particle_radius = 0.75f,
    //             .set_life_time = G_ParticleSystem::Collision::SLT_Start_animation,
    //             .move_speed = -0.95f
    //         }
    //     );
    //     pPS = &ps;
    //     ps.SetDrawState(true);
    //     ps.AddCollisionObject(1.f, { 0.f, 0.f, 1.26f });    //  set collision object to player start pos

        // float tiles_per_second = 7.f;
        // // float width = 30.f;      //  flame
        // // float height = 30.f;     //  flame
        // float width = 2.f;         //  buuble
        // float height = 2.f;        //  buuble
        // float secs_to_start_max = 3.f;  //  takes random from 0 to secs_to_start_max

        // float color_appear_secs = 0.8f;           //  BUBBLE
        // float color_disappear_secs = 1.f;         //  BUBBLE
        // // float color_appear_secs = 0.2f;        //  FLAME
        // // float color_disappear_secs = 0.6f;     //  FLAME

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
        // float speed_power = 1.f;
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


//  need to edit ZC_GUI_TextInputWindow::Caret movement with pressed ctr and collision on spesial symbols (.,*!$ ...), in method ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown/KeyboardButtonRightDown(bool needSubData)
        // //      aaaaaa&&&.&#$   .**#
        //  ckeck what is the symbol with > < = of unicode codes! with not a == L'#' !!!
        // enum Chars
        // {
        //     Ch_symbol,
        //     Ch_text,
        //     Ch_space
        // } cur_char;     //  void ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown(bool needSubData)