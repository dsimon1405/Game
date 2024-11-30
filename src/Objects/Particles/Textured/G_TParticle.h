#pragma once

#include <ZC/ZC_Types.h>
#include <ZC/Tools/Math/ZC_Vec3.h>
#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

struct SetupParticle;

struct G_PS_Source
{
        //  Determines drawing params (fbo, draw level), and get Texture with tile params.
    struct Render
    {
        ZC_RenderLevel render_level = ZC_RL_Default;
        ZC_DrawerLevel drawer_level;

        std::string tex_path;   //  fool path to the texture
            //  for calculation tiles coords in the rexture 
        int columns_count = 1;
        int rows_count = 1;
    };
        //  Describes the shape of the figure in which the particles will begin their life.
    struct SpawnShape
    {
        enum Shape
        {               //  Appear shape params:
            S__Circle = 0,     //  val_1 - radius_min; val_2 - radius_max; val_3 - unused. Fill circle with random values on range [radius_min, radius_max].
            S__Sphere = 1,     //  val_1 - radius_min; val_2 - radius_max; val_3 - unused. Fill sphere with random values on range [radius_min, radius_max].
            S__Quad = 2,       //  val_1 - length_X; val_2 - length_Y; val_3 - length_Z. Fill quad with random values on x, y, z.
        };

        Shape shape = S__Circle;
        f_zc val_1 = 0.f;   //  see enum Shape
        f_zc val_2 = 0.f;   //  see enum Shape
        f_zc val_3 = 0.f;   //  see enum Shape
    };
        //  Parameters of model matrix to manipulate SpawnShape to the world space.
    struct SpawnMatModel
    {
            //  translate
        ZC_Vec3<f_zc> position;             //  ParticleSystem::mat_model - ZC_Mat4<float>(1.f).Tanslate(translate).Rotate(rotation_angle, rotation_axis_power).Scale(scale)
            //  rotate
        f_zc rotation_angle = 0.f;              //  ParticleSystem::mat_model - ZC_Mat4<float>(1.f).Tanslate(translate).Rotate(rotation_angle, rotation_axis_power).Scale(scale)
        ZC_Vec3<f_zc> rotation_axis_power;      //  ParticleSystem::mat_model - ZC_Mat4<float>(1.f).Tanslate(translate).Rotate(rotation_angle, rotation_axis_power).Scale(scale)
    };
        //  Particle size
    struct Size
    {
        float width = 1.f;
        float height = 1.f;
    };
        //  Parrticle life time params.
    struct LifeTime
    {
        f_zc secs_to_start_max = 0.f;   //  max seconds to start particle's life. Particle::secs_to_start - takes random [0, secs_to_start_max]
        f_zc secs_min = 1.f;    //  particle life time calculation param. Particle::life_secs_total takes random [secs_min, secs_max]
        f_zc secs_max = 1.f;    //  particle life time calculation param. Particle::life_secs_total takes random [secs_min, secs_max]
    };
        //  Particle alpha channel params.
    struct Visibility
    {
        float appear_secs = 0.f;        //  particle appear during this time. Counted from the particle's respawn
        float disappear_secs = 0.f;     //  particle disappears during this time. Start disappear from -> Particle::life_secs_total - disappear_secs
    };
        //  Particle move params.
    struct Move
    {
            //  model of direction for each particle
        enum DirectionType
        {
            DT__from_particles_center    = 0,    //  Default direction, ignore G_ParticleMoveSet::move_variable. Use the normalized Particle::pos_start (particles local space diraction) as the direction of movement. If G_PLS__world_space is used, when a particle spawns, local space direction will rotate (just rotate!) with ParticleSystem::mat_model to have the actual direction at start.
            DT__variable_is_direction    = 1,    //  All particles will have the same move direction sets at G_ParticleMoveSet::move_variable (direction in particles local space) when spawned. If G_PLS__world_space is used, when a particle spawns, local space direction will rotate (just rotate!) with ParticleSystem::mat_model to have the actual direction at start.
            DT__variable_is_destination  = 2,    //  G_ParticleMoveSet::move_variable is final destination (in particles local space) for all particles for wich each particle calculate direction. Destination does not rotates, no metter woch uses LS__particle_system LS__world
        };

        DirectionType direction_type = DT__from_particles_center;   //  help calculate Particle::dir_move_normalized
        ZC_Vec3<float> move_variable;       //  see DT__variable_is_direction, DT__variable_is_destination
        float speed_power = 1.f;       //  coefficient of movement speed of all particles. ParticleSystem::speed_power. Speed power of all particles (multiplied by the individual speed of each particle). If 0 particles do not move.
        float speed_min = 1.f;         //  particle move speed calculate param. ParticleSystem::move_speed_secs - takes random [speed_min, speed_max]
        float speed_max = 1.f;         //  particle move speed calculate param. ParticleSystem::move_speed_secs - takes random [speed_min, speed_max]
    };
        //  Animation RenderData::texture params
    struct Animation
    {
        enum ChangeTilesStyle
        {
            CHT__Loop = 0,           //  changes tiles in a circle despite spawn
            CHT__Single_pass = 1,    //  passes tiles once during their lifetime
        };

        enum OffsetFrom
        {
            OF__Start = 0,
            OF__End = 1
        };

        ChangeTilesStyle change_tyles_style = CHT__Loop;
        float tiles_per_second = 0.f;     //  how much tiles change in second. ParticleSystem::uv_shift_speed = 1 / tiles_per_second
        
        OffsetFrom offset_from = OF__Start;     //  make an offset (secs_offset_to_start_animation) from the beginning of the particle's lifetime or from the end
        float secs_offset_to_start_animation = 0.f;      //  calculates seconds to start animation, while not start shows first tile
    };

    enum LifeSpace
    {
        LS__particle_system = 0,    //  particles moves in the particles local space in compute shader and then in the geometry shader with ParticleSystem::mat_model matrix translates to the world position (if mat_model changes all particles will change there positions).
        LS__world = 1,    //  when spawned, each particle gets a world position by multiplying ParticleSystem::mat_model in the compute shader, then moves while it is alive in the compute shader (changing mat_model does not affect particles that are already alive, only particles that will reborn).
    };

    ul_zc particles_count = 1;
    LifeSpace life_space = LS__particle_system;     //  ParticleSystem::life_space
    Render render;
    SpawnShape spawn_shape;
    SpawnMatModel spawn_mat_model;
    Size size;
    LifeTime life_time;
    Visibility visibility;
    Move move;
    Animation animation;
};

class G_ParticleSystem
{
public:
    ZC_uptr<SetupParticle> upSP;


        
#define G_BIND_SSBO_PARTICLE 0
#define G_BIND_SSBO_TEX_DATA 1
    struct UV   //  array of UV into SSBO_UV on gpu
    {
        float left_x = 0.f;
        float top_y = 0.f;
        float right_x = 0.f;
        float bottom_y = 0.f;
    };
    
        //  particle
    struct Particle    //  std 430 to avoid problems with alignment, don't use mat and vec types!
    {
        float life_secs_total = 0.f;
        float life_secs_cur = 0.f;

        float life_time_alpha = 0.f;

        float secs_to_start = 0.f;

            //  position
        ZC_Vec3<float> pos_start;
        ZC_Vec3<float> pos_cur;
        
        ZC_Vec3<float> dir_move_normalized;
        float move_speed_secs = 0.f;

            //  animation, uv
        uint uvs_id = 0u;
        uint uvs_cur_id = 0u;
    };
    struct ParticleSystem    //  SSBO_Particles on gpu
    {
                //  Update every frame on the cpu
            //  time
        float prev_frame_secs = 0.f;
        float total_secs = 0.f;   //  seconds from the start of particle system drawing
                //  may be changed
            //  origin pos
        ZC_Mat4<float> mat_model;   //  may be located in other SSBOs and one calculated system may be use in different places

                //  Update only on configuration
            //  corners rotated face to cam
        ZC_Vec3<float> bl;
        ZC_Vec3<float> br;
        ZC_Vec3<float> tl;
        ZC_Vec3<float> tr;
            //  particle size for corners calculation
        float half_width = 0.f;
        float half_height = 0.f;
            //  alpha data
        float appear_secs = 0.f;
        float disappear_secs = 0.f;

            //  position
        G_PS_Source::LifeSpace life_space;     //  look enum G_ParticleLifeSpace
                //  move
        
        float speed_power = 0.f;     //  total move speed of all particles
        
            //  Animation
        float uv_shift_speed = 0.f;   //  how much change tiles in second

        // Particle particles[];    AT GPU
    };


    G_ParticleSystem(const G_PS_Source& _ps_source);
    ~G_ParticleSystem();

    void SetDrawState(bool need_draw);
    void Set_Particles_count(ul_zc count);
    void Set_Life_space(G_PS_Source::LifeSpace life_space);
    void Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape shape);
    void Set_SpawnShape__variable_1(float var_1);
    void Set_SpawnShape__variable_2(float var_2);
    void Set_SpawnShape__variable_3(float var_3);
    void Set_SpawnMatModel__position(const ZC_Vec3<float>& pos);
    void Set_SpawnMatModel__rotation(float angle, const ZC_Vec3<float>& axises);
    void Set_Size__widht(float widht);
    void Set_Size__height(float height);
    void Set_Life_time__secs_to_start_max(float start_at_sec);
    void Set_Life_time__min(float min_sec);
    void Set_Life_time__max(float max_sec);
    void Set_Visibility__start(float start_sec);
    void Set_Visibility__end(float end_sec);
    void Set_Move__direction_type(G_PS_Source::Move::DirectionType direction_type);
    void Set_Move__move_variable(const ZC_Vec3<float>& move_variable);
    void Set_Move__speed_power(float speed_power);
    void Set_Move__speed_min(float speed_min_secs);
    void Set_Move__speed_max(float speed_max_secs);
    void Set_Animation__change_tyles_style(G_PS_Source::Animation::ChangeTilesStyle change_tyles_style);
    void Set_Animation__tiles_per_second(float tiles_per_second);
    void Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom offset_from);
    void Set_Animation__secs_offset_to_start_animation(float secs_offset_to_start_animation);

    static float GetRandom(float secs_min, float secs_max);
    
// private:
    static inline const float f_100 = 100.f;
    static inline const int i_100 = 100;
    // std::vector<ZC_Vec4<float>> pos_and_life_time;

    G_PS_Source ps_src;
    ParticleSystem particle_system;
    std::vector<Particle> particles;

    const G_PS_Source& c_ps_src = ps_src;
    const ParticleSystem& c_particles_data = particle_system;

    ZC_DrawerSet ds;
    ZC_DSController ds_con;

    ZC_EC ec_updater;

    // ZC_Mat4<float> unData;

    ParticleSystem CreateParticleSystem();
    void FillParticlePositoin(Particle& rParticle, const ZC_Vec3<float>& pos_start);

    std::vector<Particle> FillParticles();
    void FillParticlesPositions(std::vector<Particle>& rParticles);
    void FillShapeSphera(std::vector<Particle>& rParticles);
    // ZC_Vec2<float> CalcRandPosXYInCircle();
    void FillShapeCircle(std::vector<Particle>& rParticles);

    ZC_DrawerSet CreateDrawerSet();

    void Callback_Updater(float time);
};



#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <System/G_RenderLevel.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/GUI/ZC_GUI__Switch.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/GUI/ZC__GUI.h>
struct SetupParticle
{
    G_ParticleSystem* pPS = nullptr;
    // struct TestParams
    // {
    //     float tiles_per_second = 7.f;
    //     // float width = 30.f;      //  flame
    //     // float height = 30.f;     //  flame
    //     float width = 2.f;         //  buuble
    //     float height = 2.f;        //  buuble
    //     float secs_to_start_max = 3.f;  //  takes random from 0 to secs_to_start_max

    //     float appear_secs = 0.8f;           //  BUBBLE
    //     float disappear_secs = 1.f;         //  BUBBLE
    //     // float appear_secs = 0.2f;        //  FLAME
    //     // float disappear_secs = 0.6f;     //  FLAME

    //     float pos_x = 0.f;
    //     float pos_y = 0.f;
    //     float pos_z = 5.f;
    //     float rot_angle = 0.f;
    //     float rot_axis_x = 0.f;
    //     float rot_axis_y = 0.f;
    //     float rot_axis_z = 0.f;

    //     float life_secs_min = 2.f;
    //     float life_secs_max = 10.f;          //  BUBBLE
    //     // float life_secs_max = 3.f;       //  FLAME

    //     float circle_radius = 10.f;         //  BUBBLE
    //     float particles_count = 1.f;        //  BUBBLE
    //     // float circle_radius = 5.f;       //  FLAME
    //     // float particles_count = 18.f;    //  FLAME

    //     // float move_to_x = 0.f;
    //     // float move_to_y = 0.f;
    //     // float move_to_z = 6.f;
    //     float speed_power = 1.f;
    //     float move_speed_min_sec = 4.f;
    //     float move_speed_max_sec = 13.f;         //  BUBBLE
    //     // float move_speed_max_sec = 7.f;      //  FLAME
    //     G_PS_Source::LifeSpace life_space = G_PS_Source::LifeSpace::LS__world;
    // } data;

    ZC_GUI__WinImmutable win;

    ZC_GUI__ButtonNumberText<ui_zc> bnt__particles_count;
    ZC_GUI__Text t__life_space;
    ZC_GUI__SwitchDropDown sdd__life_space;

    std::wstring wstr_radius_min = L"radius min";
    std::wstring wstr_radius_max = L"radius max";
    std::wstring wstr_lenght_X = L"lenght X";
    std::wstring wstr_lenght_Y = L"lenght Y";
    ZC_GUI__Text t__spawn_shape;
    ZC_GUI__SwitchDropDown sdd__spawn_shape__shape;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_shape__val_1;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_shape__val_2;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_shape__val_3;

    ZC_GUI__Text t__mat_model;
    ZC_GUI__Text t__mat_model__tanslate;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__translate_x;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__translate_y;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__translate_z;
    ZC_GUI__Text t__mat_model__rotate;
    ZC_GUI__ButtonNumberText<float> mat__mat_model__rotation_angle;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__rotation_axis_power_x;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__rotation_axis_power_y;
    ZC_GUI__ButtonNumberText<float> bnt__mat_model__rotation_axis_power_z;

    ZC_GUI__Text t__size;
    ZC_GUI__ButtonNumberText<float> bnt__size__width;
    ZC_GUI__ButtonNumberText<float> bnt__size__height;

    ZC_GUI__Text t__life_time;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_to_start_max;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_min;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_max;

    ZC_GUI__Text t__visibility;
    ZC_GUI__ButtonNumberText<float> bnt__alpha__appear_secs;
    ZC_GUI__ButtonNumberText<float> bnt__alpha__disappear_secs;

    ZC_GUI__Text t__move_set;
    ZC_GUI__Text t__move_set__direction_type;
    ZC_GUI__SwitchDropDown sdd__move_set__direction_type;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_x;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_y;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_z;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_power;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_min;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_max;

    ZC_GUI__Text t__animation;
    ZC_GUI__Text t__animation__change_tyles_style;
    ZC_GUI__SwitchDropDown sdd__animation__change_tyles_style;
    ZC_GUI__ButtonNumberText<float> bnt__animation__tiles_per_second;
    ZC_GUI__Text t__animation__offset_from;
    ZC_GUI__SwitchDropDown sdd__animation__offset_from;
    ZC_GUI__ButtonNumberText<float> bnt__animation__secs_offset_to_start_animation;
    
    SetupParticle(G_ParticleSystem* _pPS)
        : pPS(_pPS),
        win(ZC_WOIData(300.f, 600.f, 0.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__Movable | ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
        bnt__particles_count(ZC_GUI_ButtonNumber<ui_zc>(0.f, 0.f, pPS->c_ps_src.particles_count, 1, 10000, 1, 2, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Particles_count, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Particles_count", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__life_space(L"Life space", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__life_space({ L"Local", L"World" }, pPS->c_ps_src.life_space, 0.f, 0.f, { &SetupParticle::Life_space, this }, ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60), ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200), ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
        t__spawn_shape(L"Spawn Shape ", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__spawn_shape__shape({ L"Circle", L"Sphere", L"Quad" }, pPS->c_ps_src.life_space, 0.f, 0.f, { &SetupParticle::Spawn_shape__shape, this }, ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60), ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200), ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
        bnt__spawn_shape__val_1(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_shape.val_1, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Spawn_shape__val_1, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), (pPS->c_ps_src.spawn_shape.shape == G_PS_Source::SpawnShape::S__Quad ? wstr_lenght_X : wstr_radius_min), false, ZC__GUI::CalculateWstrWidth(wstr_radius_min), ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__spawn_shape__val_2(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_shape.val_2, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Spawn_shape__val_2, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), (pPS->c_ps_src.spawn_shape.shape == G_PS_Source::SpawnShape::S__Quad ? wstr_lenght_Y : wstr_radius_max), false, ZC__GUI::CalculateWstrWidth(wstr_radius_max), ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__spawn_shape__val_3(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_shape.val_3, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Spawn_shape__val_3, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"length Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__mat_model(L"Spawn Mat Model", true, 0, ZC_GUI_TextAlignment::Left),
        t__mat_model__tanslate(L"Translate", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__mat_model__translate_x(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.position[0], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__position_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__mat_model__translate_y(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.position[1], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__position_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__mat_model__translate_z(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.position[2], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__position_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__mat_model__rotate(L"Rotate", true, 0, ZC_GUI_TextAlignment::Left),
        mat__mat_model__rotation_angle(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotation_angle, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__rotation_angle, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Angle", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__mat_model__rotation_axis_power_x(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotation_axis_power[0], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__rotation_axis_power_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__mat_model__rotation_axis_power_y(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotation_axis_power[1], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__rotation_axis_power_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__mat_model__rotation_axis_power_z(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotation_axis_power[2], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Mat_model__rotation_axis_power_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__size(L"Size", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__size__width(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.size.width, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Size__width, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Width", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__size__height(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.size.height, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Size__height, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Height", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__life_time(L"Life Time (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__life_time__secs_to_start_max(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.life_time.secs_to_start_max, 0.1f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Life_time__secs_to_start_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Life start at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__life_time__secs_min(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.life_time.secs_min, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Life_time__secs_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Duration min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__life_time__secs_max(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.life_time.secs_max, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Life_time__secs_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Duration max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__visibility(L"Visibility (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__alpha__appear_secs(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.visibility.appear_secs, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Alpha__appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Appear", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__alpha__disappear_secs(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.visibility.disappear_secs, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Alpha__disappear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Disappear", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__move_set(L"Move (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        t__move_set__direction_type(L"Move type (system space)", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__move_set__direction_type({ L"From center", L"Direction", L"Point" }, pPS->c_ps_src.move.direction_type, 0.f, 0.f, { &SetupParticle::Move_set__direction_type, this }, ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60), ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200), ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
        bnt__move_set__move_variable_x(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.move_variable[0], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__move_variable_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__move_set__move_variable_y(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.move_variable[1], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__move_variable_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__move_set__move_variable_z(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.move_variable[2], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__move_variable_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__move_set__speed_power(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.speed_power, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__speed_power, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Speed power", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__move_set__speed_min(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.speed_min, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__speed_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Speed min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt__move_set__speed_max(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.move.speed_max, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Move_set__speed_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Speed max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__animation(L"Animation (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        t__animation__change_tyles_style(L"Style", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__animation__change_tyles_style({ L"Loop", L"Single pass" }, pPS->c_ps_src.animation.change_tyles_style, 0.f, 0.f, { &SetupParticle::Animation__change_tyles_style, this }, ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60), ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200), ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
        bnt__animation__tiles_per_second(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.animation.tiles_per_second, 0.f, 1000.f, 1.f, 1.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Animation__tiles_per_second, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Tiles per second", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        t__animation__offset_from(L"Offset from life", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__animation__offset_from({ L"Start", L"End" }, pPS->c_ps_src.animation.offset_from, 0.f, 0.f, { &SetupParticle::Animation__offset_from, this }, ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60), ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200), ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
        bnt__animation__secs_offset_to_start_animation(ZC_GUI_ButtonNumber<float>(0.f, 0.f, pPS->c_ps_src.animation.secs_offset_to_start_animation, 0.f, 1000.f, 1.f, 1.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Animation__secs_offset_to_start_animation, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"Start animation at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0)))
    {
        float section_y = 20;
        float sub_section_y = 10;
        float row_y = 5;
        float distacne_x = 10.f;
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__particles_count.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, distacne_x), { sdd__life_space.GetObj(), t__life_space.GetObj() }));

        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__spawn_shape.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { sdd__spawn_shape__shape.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__spawn_shape__val_1.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__spawn_shape__val_2.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__spawn_shape__val_3.GetObj() }));
        if (pPS->c_ps_src.spawn_shape.shape != G_PS_Source::SpawnShape::S__Quad) bnt__spawn_shape__val_3.SetDrawState(false);

        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__mat_model.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, sub_section_y, 0.f), { t__mat_model__tanslate.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__translate_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__translate_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__translate_z.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, sub_section_y, 0.f), { t__mat_model__rotate.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { mat__mat_model__rotation_angle.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__rotation_axis_power_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__rotation_axis_power_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__mat_model__rotation_axis_power_z.GetObj() }));
        
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__size.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__size__width.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__size__height.GetObj() }));

        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__life_time.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__life_time__secs_to_start_max.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__life_time__secs_min.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__life_time__secs_max.GetObj() }));

        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__visibility.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__alpha__appear_secs.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__alpha__disappear_secs.GetObj() }));
        
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__move_set.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, sub_section_y, distacne_x), { sdd__move_set__direction_type.GetObj(), t__move_set__direction_type.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__move_set__move_variable_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__move_set__move_variable_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__move_set__move_variable_z.GetObj() }));
        if (pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center)
        {
            bnt__move_set__move_variable_x.SetDrawState(false);
            bnt__move_set__move_variable_y.SetDrawState(false);
            bnt__move_set__move_variable_z.SetDrawState(false);
        }
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, sub_section_y, 0.f), { bnt__move_set__speed_power.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__move_set__speed_min.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__move_set__speed_max.GetObj() }));

        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, section_y, 0.f), { t__animation.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, distacne_x), { sdd__animation__change_tyles_style.GetObj(), t__animation__change_tyles_style.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__animation__tiles_per_second.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, distacne_x), { sdd__animation__offset_from.GetObj(), t__animation__offset_from.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, row_y, 0.f), { bnt__animation__secs_offset_to_start_animation.GetObj() }));
    }
    
    void Particles_count(ui_zc v) { pPS->Set_Particles_count(v); }
    void Life_space(ui_zc v) { pPS->Set_Life_space(G_PS_Source::LifeSpace(v)); }
    void Spawn_shape__shape(ui_zc v)
    {
        if (v == G_PS_Source::SpawnShape::S__Quad)
        {
            bnt__spawn_shape__val_1.UpdateText(wstr_lenght_X, true);
            bnt__spawn_shape__val_2.UpdateText(wstr_lenght_Y, true);
            bnt__spawn_shape__val_3.SetDrawState(true);
        }
        else
        {
            bnt__spawn_shape__val_1.UpdateText(wstr_radius_min, true);
            bnt__spawn_shape__val_2.UpdateText(wstr_radius_max, true);
            bnt__spawn_shape__val_3.SetDrawState(false);
        }
    
        pPS->Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape(v));
    }
    void Spawn_shape__val_1(float v) { pPS->Set_SpawnShape__variable_1(v); }
    void Spawn_shape__val_2(float v) { pPS->Set_SpawnShape__variable_2(v); }
    void Spawn_shape__val_3(float v) { pPS->Set_SpawnShape__variable_2(v); }
    
    void Mat_model__position_x(float v) { pPS->Set_SpawnMatModel__position({ v, pPS->c_ps_src.spawn_mat_model.position[1], pPS->c_ps_src.spawn_mat_model.position[2] }); }
    void Mat_model__position_y(float v) { pPS->Set_SpawnMatModel__position({ pPS->c_ps_src.spawn_mat_model.position[0], v, pPS->c_ps_src.spawn_mat_model.position[2] }); }
    void Mat_model__position_z(float v) { pPS->Set_SpawnMatModel__position({ pPS->c_ps_src.spawn_mat_model.position[0], pPS->c_ps_src.spawn_mat_model.position[1], v }); }
    void Mat_model__rotation_angle(float v) { pPS->Set_SpawnMatModel__rotation(v, pPS->c_ps_src.spawn_mat_model.rotation_axis_power); }
    void Mat_model__rotation_axis_power_x(float v) { pPS->Set_SpawnMatModel__rotation(pPS->c_ps_src.spawn_mat_model.rotation_angle,
        { v, pPS->c_ps_src.spawn_mat_model.rotation_axis_power[1], pPS->c_ps_src.spawn_mat_model.rotation_axis_power[2] }); }
    void Mat_model__rotation_axis_power_y(float v) { pPS->Set_SpawnMatModel__rotation(pPS->c_ps_src.spawn_mat_model.rotation_angle,
        { pPS->c_ps_src.spawn_mat_model.rotation_axis_power[0], v, pPS->c_ps_src.spawn_mat_model.rotation_axis_power[2] }); }
    void Mat_model__rotation_axis_power_z(float v) { pPS->Set_SpawnMatModel__rotation(pPS->c_ps_src.spawn_mat_model.rotation_angle,
        { pPS->c_ps_src.spawn_mat_model.rotation_axis_power[0], pPS->c_ps_src.spawn_mat_model.rotation_axis_power[1], v }); }
    
    void Size__width(float v) { pPS->Set_Size__widht(v); }
    void Size__height(float v) { pPS->Set_Size__height(v); }
    
    void Life_time__secs_to_start_max(float v) { pPS->Set_Life_time__secs_to_start_max(v); }
    void Life_time__secs_min(float v) { pPS->Set_Life_time__min(v); }
    void Life_time__secs_max(float v) { pPS->Set_Life_time__max(v); }

    void Alpha__appear_secs(float v) { pPS->Set_Visibility__start(v); }
    void Alpha__disappear_secs(float v) { pPS->Set_Visibility__end(v); }

    void Move_set__direction_type(ui_zc v)
    {
        bnt__move_set__move_variable_x.SetDrawState(pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center);
        bnt__move_set__move_variable_y.SetDrawState(pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center);
        bnt__move_set__move_variable_z.SetDrawState(pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center);
        pPS->Set_Move__direction_type(G_PS_Source::Move::DirectionType(v));
    }
    void Move_set__move_variable_x(float v) { pPS->Set_Move__move_variable({ v, pPS->c_ps_src.move.move_variable[1], pPS->c_ps_src.move.move_variable[2] }); }
    void Move_set__move_variable_y(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.move_variable[0], v, pPS->c_ps_src.move.move_variable[2] }); }
    void Move_set__move_variable_z(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.move_variable[0], pPS->c_ps_src.move.move_variable[1], v }); }
    void Move_set__speed_power(float v) { pPS->Set_Move__speed_power(v); }
    void Move_set__speed_min(float v) { pPS->Set_Move__speed_min(v); }
    void Move_set__speed_max(float v) { pPS->Set_Move__speed_max(v); }
    
    void Animation__change_tyles_style(ui_zc v) { pPS->Set_Animation__change_tyles_style(G_PS_Source::Animation::ChangeTilesStyle(v)); }
    void Animation__tiles_per_second(float v) { pPS->Set_Animation__tiles_per_second(v); }
    void Animation__offset_from(ui_zc v) { pPS->Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom(v)); }
    void Animation__secs_offset_to_start_animation(float v) { pPS->Set_Animation__secs_offset_to_start_animation(v); }


                                    //  VAO+VBO
    // void Animation__tiles_per_second(float v)
    // {
    //     data.tiles_per_second = v;
    //     G_ParticleSystem::pStatic->unData[0][1] = v;
    // }
    // void Width(float v)
    // {
    //     data.width = v;
    //     G_ParticleSystem::pStatic->unData[0][2] = v;
    // }
    // void Height(float v)
    // {
    //     data.height = v;
    //     G_ParticleSystem::pStatic->unData[0][3] = v;
    // }
    // void Alpha__appear_secs(float v)
    // {
    //     data.appear_secs = v;
    //     G_ParticleSystem::pStatic->unData[1][0] = v;
    // }
    // void Mat_model__position_x(float v)
    // {
    //     data.pos_x = v;
    //     G_ParticleSystem::pStatic->unData[1][1] = v;
    // }
    // void Mat_model__position_x(float v)
    // {
    //     data.pos_y = v;
    //     G_ParticleSystem::pStatic->unData[1][2] = v;
    // }
    // void Mat_model__position_x(float v)
    // {
    //     data.pos_z = v;
    //     G_ParticleSystem::pStatic->unData[1][3] = v;
    // }
    // void Life_time__secs_min(float v)
    // {
    //     data.life_secs_min = v;
    //     for (ZC_Vec4<float>& plt : G_ParticleSystem::pStatic->pos_and_life_time) plt[3] = G_ParticleSystem::GetRandom(v, data.life_secs_max);
    //     G_ParticleSystem::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_ParticleSystem::pStatic->pos_and_life_time.size(), G_ParticleSystem::pStatic->pos_and_life_time.data());
    // }
    // void Life_time__secs_max(float v)
    // {
    //     data.life_secs_max = v;
    //     for (ZC_Vec4<float>& plt : G_ParticleSystem::pStatic->pos_and_life_time) plt[3] = G_ParticleSystem::GetRandom(data.life_secs_min, v);
    //     G_ParticleSystem::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_ParticleSystem::pStatic->pos_and_life_time.size(), G_ParticleSystem::pStatic->pos_and_life_time.data());
    // }
    // void Spawn_shape__val_1(float v)
    // {
    //     data.circle_radius = v;
    //     for (ZC_Vec4<float>& plt : G_ParticleSystem::pStatic->pos_and_life_time)
    //     {
    //         ZC_Vec2<float> pos_xy = G_ParticleSystem::CalcRandPosXYInCircle(v);
    //         plt = ZC_Vec4<float>(pos_xy[0], pos_xy[1], 0.f, plt[3]);
    //     }
    //     G_ParticleSystem::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_ParticleSystem::pStatic->pos_and_life_time.size(), G_ParticleSystem::pStatic->pos_and_life_time.data());
    // }
    // void Particles_count(float v)
    // {
    //     data.particles_count = v;
    //     std::vector<ZC_Vec4<float>> pos_and_life_time;
    //     pos_and_life_time.reserve(v);
    //     for (ZC_Vec4<float>& plt : G_ParticleSystem::pStatic->pos_and_life_time)     //  copy existing data
    //     {
    //         if (pos_and_life_time.capacity() == pos_and_life_time.size()) break;
    //         pos_and_life_time.emplace_back(plt);
    //     }
    //     if (pos_and_life_time.capacity() != pos_and_life_time.size())   //  if existing data not enought, create new
    //     {
    //         for (ul_zc i = pos_and_life_time.size(); i < pos_and_life_time.capacity(); ++i)
    //         {
    //             ZC_Vec2<float> pos_xy = G_ParticleSystem::CalcRandPosXYInCircle(data.circle_radius);
    //             pos_and_life_time.emplace_back(ZC_Vec4<float>(pos_xy[0], pos_xy[1], 0.f, G_ParticleSystem::GetRandom(data.life_secs_min, data.life_secs_max)));
    //         }
    //     }

    //     bool add_to_render = G_ParticleSystem::pStatic->ds_con.IsDrawing(ZC_RL_Default);
    //     if (add_to_render) G_ParticleSystem::pStatic->ds_con.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_None);    //  will be added back to the end of the method to update pointers to ZC_GLDraw, ZC_VAO in ZC_DrawerFL

    //     ul_zc particles_count = pos_and_life_time.size();
    //     ZC_Buffer vbo(GL_ARRAY_BUFFER);
    //     vbo.GLNamedBufferStorage(particles_count * sizeof(ZC_Vec4<float>), pos_and_life_time.data(), GL_DYNAMIC_STORAGE_BIT);

    //     ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, particles_count);

    //     ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

    //     ZC_VAO vao;
    //     // vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, 0);
    //     vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, particles_count);

    //     G_ParticleSystem::pStatic->ds.upGLDraw = std::move(upDraw);
    //     G_ParticleSystem::pStatic->ds.vao = std::move(vao);
    //     G_ParticleSystem::pStatic->ds.buffers.front() = std::move(vbo);

    //     G_ParticleSystem::pStatic->ds_con.pGLDraw = G_ParticleSystem::pStatic->ds.upGLDraw.Get();
    //     G_ParticleSystem::pStatic->ds_con.pVAO = &(G_ParticleSystem::pStatic->ds.vao);

    //     G_ParticleSystem::pStatic->pos_and_life_time = std::move(pos_and_life_time);

    //     if (add_to_render) G_ParticleSystem::pStatic->ds_con.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_ParticleFlame);
    // }
    // void Move_set__move_variable_x(float v)
    // {
    //     data.move_to_x = v;
    //     G_ParticleSystem::pStatic->unData[2][0] = v;
    // }
    // void Move_set__move_variable_y(float v)
    // {
    //     data.move_to_y = v;
    //     G_ParticleSystem::pStatic->unData[2][1] = v;
    // }
    // void Move_set__move_variable_z(float v)
    // {
    //     data.move_to_z = v;
    //     G_ParticleSystem::pStatic->unData[2][2] = v;
    // }
    // void Move_set__speed_min(float v)
    // {
    //     data.move_speed_min_sec = v;
    //     G_ParticleSystem::pStatic->unData[2][3] = ZC_Random::GetRandomInt(data.move_speed_min_sec * G_ParticleSystem::f_100, data.move_speed_max_sec * G_ParticleSystem::f_100) / G_ParticleSystem::f_100;
    // }
    // void Move_set__speed_max(float v)
    // {
    //     data.move_speed_max_sec = v;
    //     G_ParticleSystem::pStatic->unData[2][3] = ZC_Random::GetRandomInt(data.move_speed_min_sec * G_ParticleSystem::f_100, data.move_speed_max_sec * G_ParticleSystem::f_100) / G_ParticleSystem::f_100;
    // }
};