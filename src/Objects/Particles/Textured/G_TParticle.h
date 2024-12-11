#pragma once

#include <ZC/ZC_Types.h>
#include <ZC/Tools/Math/ZC_Vec3.h>
#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

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
        {
            S__Circle = 0,
            S__Sphere,
            S__Square,
            S__Cube,
            S__Hemisphere
        };

        Shape shape = S__Circle;
            //  wich persent of the shape must be filled. Range [0,1], more closer to 1 then more part of shape will be filled. Filling starts from the edge of the shape.
            //  Example with S__Circle: 0 will fill only the border of the circle; 0.5 will fill half radius of the circle, counting from the edge; 1 will fill the entire area of ​​the circle.cle.
        f_zc fill_to_center = 0.f;
    };
        //  Parameters of model matrix to manipulate SpawnShape to the world space.
    struct SpawnMatModel
    {
        enum RotateOrder
        {
            RO_xyz,
            RO_xzy,
            RO_yxz,
            RO_zxy,
            RO_yzx,
            RO_zyx,
        };
            //  translate
        ZC_Vec3<f_zc> translate;        //  ParticleSystem::spawn_mat_model - ZC_Mat4<float>(1.f).Tanslate().Rotate().Rotate().Rotate().Scale()
            //  rotate
        RotateOrder rotate_order = RO_xyz;
        f_zc rotate_angle_X = 0.f;        //  ParticleSystem::spawn_mat_model - ZC_Mat4<float>(1.f).Tanslate().Rotate().Rotate().Rotate().Scale()
        f_zc rotate_angle_Y = 0.f;        //  ParticleSystem::spawn_mat_model - ZC_Mat4<float>(1.f).Tanslate().Rotate().Rotate().Rotate().Scale()
        f_zc rotate_angle_Z = 0.f;        //  ParticleSystem::spawn_mat_model - ZC_Mat4<float>(1.f).Tanslate().Rotate().Rotate().Rotate().Scale()
            //  scale
        ZC_Vec3<float> scale;        //  ParticleSystem::spawn_mat_model - ZC_Mat4<float>(1.f).Tanslate().Rotate().Rotate().Rotate().Scale()
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
        f_zc secs_to_start_max = 0.f;   //  max seconds to start particle's life. Particle::life_time_secs_to_start - takes random [0, secs_to_start_max]
        f_zc secs_min = 1.f;    //  particle life time calculation param. Particle::life_time_secs_total takes random [secs_min, secs_max]
        f_zc secs_max = 1.f;    //  particle life time calculation param. Particle::life_time_secs_total takes random [secs_min, secs_max]
    };
        //  Particle move params.
    struct Move
    {
            //  model of direction for each particle
        enum DirectionType
        {
            DT__from_particles_center    = 0,    //  Default direction, ignore Move::variable. On gpu use the normalized Particle::pos_start (particles local space diraction) as the direction of movement. If G_PLS__world_space is used, when a particle spawns, local space direction will rotate (just rotate!) with ParticleSystem::spawn_mat_model to have the actual direction at start.
            DT__variable_is_direction    = 1,    //  All particles will have the same move direction sets at G_ParticleMoveSet::variable (direction in particles local space) when spawned. If G_PLS__world_space is used, when a particle spawns, local space direction will rotate (just rotate!) with ParticleSystem::spawn_mat_model to have the actual direction at start.
            DT__variable_is_destination  = 2,    //  G_ParticleMoveSet::variable is final destination (in particles local space) for all particles for wich each particle calculate direction. Destination does not rotates, no metter woch uses LS__particle_system LS__world
        };
        DirectionType direction_type = DT__from_particles_center;   //  help calculate Particle::move_dir_normalized
        ZC_Vec3<float> variable;       //  see DT__variable_is_direction, DT__variable_is_destination
        float speed_power = 1.f;       //  coefficient of movement speed of all particles. ParticleSystem::speed_power. Speed power of all particles (multiplied by the individual speed of each particle). If 0 particles do not move.
        float speed_min = 1.f;         //  particle move speed calculate param. ParticleSystem::move_speed_secs_start - takes random [speed_min, speed_max]
        float speed_max = 1.f;         //  particle move speed calculate param. ParticleSystem::move_speed_secs_start - takes random [speed_min, speed_max]
    };
        //  Rotate particle in 2d
    struct Rotate
    {
        enum AngleUse
        {
            AU__contant,    //  each particle will have angle_1
            AU__random_between_constants,   //  each particle will have angle_1 or angle_2
            AU__random_between_angles,      //  each particle will have random angle between angle_1 angle_2
        } angle_use = AU__contant;

        float angle_1 = 0.f;    //  angle in degrees, see enum AngleUse
        float angle_2 = 0.f;    //  angle in degrees, see enum AngleUse

        bool operator == (const Rotate& r) const noexcept;
        float GetAngle();
    };
        //  Animation RenderData::texture params
    struct Animation
    {
        enum Repaet
        {
            R_Loop = 0,           //  changes tiles in a circle despite spawn
            R_Single_pass = 1,    //  passes tiles once during their lifetime
        };

        enum OffsetFrom
        {
            OF__Start = 0,  //  take offset from life time start to start animation
            OF__End = 1     //  take offset from life time end to start animation 
        };

        Repaet repaet = R_Loop;
        float tiles_per_second = 0.f;     //  how much tiles change in second. ParticleSystem::animation_uv_shift_speed = 1 / tiles_per_second
        
        OffsetFrom offset_from = OF__Start;     //  make an offset (offset_to_start_animation_secs) from the beginning of the particle's lifetime or from the end
        float offset_to_start_animation_secs = 0.f;      //  calculates seconds to start animation, while not start shows first tile

        float Calc_animation_start_secs(float life_time_secs_total);
    };
        //  Color added to particle
    struct Color
    {
        enum RGBUse
        {
            RGBU_Add,      //  color texture + calculated rgb color
            RGBU_Replace   //  don't take texture rgb color at all, only calculated color
        };

        RGBUse rgb_use = RGBU_Add;     //  how to use calculated rgb color
        float appear_secs = 0.f;        //  particle appear during this time. Counted from the particle's respawn
        float disappear_secs = 0.f;     //  particle disappears during this time. Start disappear from -> Particle::life_time_secs_total - color_disappear_secs
        ZC_Vec4<float> rgba_start { 0.f, 0.f, 0.f, 1.f };      //  rgba range [0, 1]. rgb interpolated over life time from start to end. Alpha interpolated with appear_secs
        ZC_Vec4<float> rgba_end { 0.f, 0.f, 0.f, 1.f };        //  rgba range [0, 1]. rgb interpolated over life time from start to end. Alpha interpolated with disappear_secs
    };

    ul_zc particles_count = 1;
    Render render;
    SpawnShape spawn_shape;
    SpawnMatModel spawn_mat_model;
    Size size;
    LifeTime life_time;
    Move move;
    Rotate rotate;
    Animation animation;
    Color color;
};

#define SETUP__G_PARTICLE_SYSTEM        //  DEBUG
#ifdef SETUP__G_PARTICLE_SYSTEM
struct Setup__G_ParticleSystem;
#endif SETUP__G_PARTICLE_SYSTEM

class G_ParticleSystem
{
public:
        //  Particle collision source. Each parameter of the structure have effect only on collision.
    struct Collision     //   on gpu ssbo Collision
    {
            //  collision
        enum SetLifeTime
        {
            SLF_None                = 0,        //  don't set any time.
            SLT_Start_animation     = 1,        //  sets Particle::life_time_secs_cur = Particle::animation_start_secs, to start the animation if it doesn't already exist.
        };

        uint collision_objects_count = 0;    //  filles with system, may be missed. Count of object in collision_objects.
        float particle_radius = 0.f;    //  radius ofthe particle (may be less then drawing particle size, help to avoid alpha channels border around the particle).

        SetLifeTime set_life_time = SLF_None;     //  cannot be changed before AddCollisionObject() is called at least once.
        float move_speed = 0.f;      //  percent value to change speed value. Range [-1.f, 1.f].
    };

    const G_PS_Source& c_ps_src = ps_src;
    const Collision& c_pc_src = pc_src;

    G_ParticleSystem(const G_PS_Source& _ps_source, const Collision& _pc_src);
    ~G_ParticleSystem();

    void SetDrawState(bool need_draw);
        //  return true if drawing, otherwise false
    bool GetDrawState();

    void Set_Particles_count(ul_zc count);
    void Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape shape);
    void Set_SpawnShape__fill_to_center(float fill_to_center);
    void Set_SpawnMatModel__translation(const ZC_Vec3<float>& trans);
    void Set_SpawnMatModel__rotate(typename G_PS_Source::SpawnMatModel::RotateOrder rotate_order, float x, float y, float z);
    void Set_SpawnMatModel__scale(const ZC_Vec3<float>& scale);
    void Set_Size__width(float width);
    void Set_Size__height(float height);
    void Set_Life_time__secs_to_start_max(float secs_to_start_max);
    void Set_Life_time__min(float secs_min);
    void Set_Life_time__max(float secs_max);
    void Set_Move__direction_type(typename G_PS_Source::Move::DirectionType direction_type);
    void Set_Move__move_variable(const ZC_Vec3<float>& variable);
    void Set_Move__speed_power(float speed_power);
    void Set_Move__speed_min(float speed_min_secs);
    void Set_Move__speed_max(float speed_max_secs);
    void Set_Rotate(const typename G_PS_Source::Rotate& rotate);
    void Set_Animation__repeat(typename G_PS_Source::Animation::Repaet repaet);
    void Set_Animation__tiles_per_second(float tiles_per_second);
    void Set_Animation__offset_from(typename G_PS_Source::Animation::OffsetFrom offset_from);
    void Set_Animation__secs_offset_to_start_animation(float offset_to_start_animation_secs);
    void Set_Color__appear_secs(float color_appear_secs);
    void Set_Color__rgb_use(typename G_PS_Source::Color::RGBUse rgb_use);
    void Set_Color__disappear_secs(float color_disappear_secs);
    void Set_Color__rgba_start(const ZC_Vec4<float>& rgba_start);
    void Set_Color__rgba_end(const ZC_Vec4<float>& rgba_end);
    
        //  update collison source
    void Add_CollisionSource(const Collision& _upPC_Source);
    /*
    Adds object for collision.

    Params:
    - radius - object's radius, collision obly by radius. May be updated with SetCollisionObject_radius().
    - world_pos - object's world pos. May be updated with SetCollisionObject_world_pos().

    Return:
    id of the added object.
    */
    ul_zc AddCollisionObject(float radius, const ZC_Vec3<float>& world_pos);
    void DeleteCollisionObject(ul_zc id);
    void SetCollisionObject_world_pos(ul_zc id, const ZC_Vec3<float>& world_pos);
    void SetCollisionObject_radius(ul_zc id, float radius);

    void Set_Collision__particle_radius(float particle_radius);
    void Set_Collision__set_life_time(typename Collision::SetLifeTime set_life_time);
    void Set_Collision__move_speed(float move_speed);

    static float GetRandom(float secs_min, float secs_max);

private:
#ifdef SETUP__G_PARTICLE_SYSTEM
    ZC_uptr<Setup__G_ParticleSystem> upSP;
#endif SETUP__G_PARTICLE_SYSTEM

#define G_BIND_SSBO_PARTICLE 0
#define G_BIND_SSBO_TEX_DATA 1
#define G_Bind_SSBO_COLLISION 2

    enum ExternalInfluence
    {
        EI_None         = 0,
        EI_Collision    = 1,    //  creates collision ssbo, may be used funnction for collision setup
    };
    typedef i_zc ExternalInfluenceMask;

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
            //  life time
        float life_time_secs_to_start = 0.f;   //  secs to start life time
        float life_time_secs_total = 0.f;      //  life time total secs
        float life_time_secs_cur = 0.f;        //  life time cur secs
            //  position
        ZC_Vec3<float> pos_start;
        ZC_Vec3<float> pos_cur;
            //  move
        ZC_Vec3<float> move_dir_normalized;
        float move_speed_secs_start = 0.f;  //  sets to move_speed_secs_cur at respawn
        float move_speed_secs_cur = 0.f;    //  cur speed
            //  coners world pos
        ZC_Vec3<float> world_bl;
        ZC_Vec3<float> world_br;
        ZC_Vec3<float> world_tl;
        ZC_Vec3<float> world_tr;
            //  rotate angle
        float rotate_angle = 0.f;    //  rotate particle in 2d
            //  animaion
        float animation_start_secs = 0.f;     //  when in life time to start animation
        uint animation_uvs_cur_id = 0;
        float animation_uvs_cur_id_secs = 0.f;      //  seconds to show animation_uvs_cur_id
            //  color
        uint color_rgba;     //  color to add, packed [32]->2x10x10x10
    };
    struct ParticleSystem    //  SSBO_Particles on gpu
    {
            //  time
        float time_prev_frame_secs;     //  cpu update
        float time_total_secs;          //  cpu update. Seconds from the start of particle system drawing
            //  spawn mat model
        ZC_Mat4<float> spawn_mat_model;   //  may be located in other SSBOs and one calculated system may be use in different places
            //  tex particle size for corners calculation
        float size_half_width = 0.f;
        float size_half_height = 0.f;
            //  move
        typename G_PS_Source::Move::DirectionType move_direction_type = G_PS_Source::Move::DirectionType::DT__from_particles_center;     //  see G_PS_Source::Move::DirectionType
        ZC_Vec3<float> move_variable;       //  see G_PS_Source::Move::DirectionType
        float move_speed_power = 0.f;       //  total move speed of all particles
            //  animation
        typename G_PS_Source::Animation::Repaet animation_repeat = G_PS_Source::Animation::R_Loop;
        float animation_uv_shift_speed = 0.f;                //  how much change tiles in second
            //  color
        typename G_PS_Source::Color::RGBUse color_rgb_use = G_PS_Source::Color::RGBU_Add;
        float color_appear_secs = 0.f;
        float color_disappear_secs = 0.f;
        uint color_rgba_start = 0;       //  rgba to packed [32]->8x8x8x8. rgb interpolated over life time from start to end. Alpha interpolated with appear_secs
        uint color_rgba_end = 0;         //  rgba to packed [32]->8x8x8x8. rgb interpolated over life time from start to end. Alpha interpolated with color_disappear_secs
            //  collision
        ExternalInfluenceMask external_influence_mask = EI_None;

        // Particle particles[];    AT GPU
    };

        //  collision
    struct CollisionObject
    {
        float radius = 0.f;   //  if radius = 0 it is free space
        ZC_Vec3<float> world_pos;
    };

    static inline const float f_100 = 100.f;
    static inline const int i_100 = 100;

    static inline float radius_or_half_length_max = 1.f;     //  max radius or length of the filling shape

    G_PS_Source ps_src;
    ParticleSystem ps;
    std::vector<Particle> particles;

    Collision pc_src;
    std::vector<CollisionObject> collision_objects;    //  each particle will be make collision with this objects in compute shader

    ZC_DrawerSet ds;
    ZC_DSController ds_con;

    bool update_gpu_collision_objects = false;
    bool update_gpu_spawn_mat_model = false;

    ZC_EC ec_updater;

    ZC_Mat4<f_zc> CreateSpawnMatModel();
    ParticleSystem CreateParticleSystem();

    std::vector<Particle> FillParticles();
    void FillParticlesPosition(std::vector<Particle>& rParticles);
    void SetAxisQuaterSigne2D(ZC_Vec3<float>& pos_xy);
    void SetAxisQuaterSigne3D(ZC_Vec3<float>& pos_xyz);
    void FillShapeCircle(std::vector<Particle>& rParticles);
    void FillShapeSphere(std::vector<Particle>& rParticles, bool hemisphere);
    void FillShapeSquare(std::vector<Particle>& rParticles);
    void FillShapeCube(std::vector<Particle>& rParticles);

    ZC_DrawerSet CreateDrawerSet();

    void SetSpawnDataToDefault();
    float CheckCollisionMoveSpeed(float move_speed);
    
    void Callback_Updater(float time);
};



#ifdef SETUP__G_PARTICLE_SYSTEM
#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <System/G_RenderLevel.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/GUI/ZC_GUI__Switch.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/GUI/ZC__GUI.h>
#include <GUI/Text/G_GUI_Fonts.h>
#include <ZC/GUI/ZC_GUI__CheckBox.h>
#include <ZC/GUI/ZC_GUI__ColorManipulator.h>

struct Setup__G_ParticleSystem
{
    G_ParticleSystem* pPS = nullptr;

    static inline float button_text_dist = 5.f;

    ZC_GUI__WinImmutable win;
        //  count
    ZC_GUI__ButtonNumberText<ui_zc> bnt__particles_count;
        //  spawn shape
    std::wstring wstr_spawn_shape__radius = L"Radius";
    std::wstring wstr_spawn_shape__half_length = L"Half length";
    ZC_GUI__Text t__spawn_shape;
    ZC_GUI__SwitchDropDown sdd__spawn_shape__shape;
    ZC_GUI__ButtonNumberText<uch_zc> bnt__spawn_shape__fill_to_center;
        //  spawn mat model
    ZC_GUI__Text t__spawn_mat_model;
    ZC_GUI__Text t__spawn_mat_model__tanslate;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_x;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_z;
    ZC_GUI__Text t__spawn_mat_model__rotate;
    ZC_GUI__SwitchDropDown sdd__spawn_mat_model__rotate_order;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_X;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_Y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_Z;
    ZC_GUI__Text t__spawn_mat_model__scale;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_X;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_Y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_Z;
        //  size
    ZC_GUI__Text t__size;
    ZC_GUI__ButtonNumberText<float> bnt__size__width;
    ZC_GUI__ButtonNumberText<float> bnt__size__height;
        //  life time
    ZC_GUI__Text t__life_time;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_to_start_max;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_min;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_max;
        //  move
    ZC_GUI__Text t__move_set;
    ZC_GUI__Text t__move_set__direction_type;
    ZC_GUI__SwitchDropDown sdd__move_set__direction_type;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_x;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_y;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_z;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_power;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_min;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_max;
        //  rotate
    ZC_GUI__Text t__rotate;
    ZC_GUI__Text t__rotate__angle_use;
    ZC_GUI__SwitchDropDown sdd__rotate__angle_use;
    ZC_GUI__ButtonNumberText<float> bnt__rotate__angle_1;
    ZC_GUI__ButtonNumberText<float> bnt__rotate__angle_2;
        //  animation
    ZC_GUI__Text t__animation;
    ZC_GUI__Text t__animation__repeat;
    ZC_GUI__SwitchDropDown sdd__animation__repeat;
    ZC_GUI__ButtonNumberText<float> bnt__animation__tiles_per_second;
    ZC_GUI__Text t__animation__offset_from;
    ZC_GUI__SwitchDropDown sdd__animation__offset_from;
    ZC_GUI__ButtonNumberText<float> bnt__animation__secs_offset_to_start_animation;
        //  color
    ZC_GUI__Text t__color;
    ZC_GUI__Text t__color__rgb_use;
    ZC_GUI__SwitchText st__color__rgb_use;
    ZC_GUI__Text t__color__start;
    ZC_GUI__ButtonNumberText<float> bnt__color__appear_secs;
    ZC_GUI__ColorManipulator cm__color__rgba_start;
    ZC_GUI__Text t__color__end;
    ZC_GUI__ButtonNumberText<float> bnt__color__disappear_secs;
    ZC_GUI__ColorManipulator cm__color__rgba_end;
        //  collision
    ZC_GUI__Text t__collision;
    ZC_GUI__ButtonNumberText<float> bnt__collision__particle_radius;
    ZC_GUI__Text t__collision__set_life_time;
    ZC_GUI__SwitchDropDown sdd__collision__set_life_time;
    ZC_GUI__ButtonNumberText<float> bnt__collision__move_speed;
    
    Setup__G_ParticleSystem(G_ParticleSystem* _pPS)
        : pPS(_pPS),
        win(ZC_WOIData(350.f, 600.f, -300.f, 0.f, ZC_WOIF__X_Center_Pixel | ZC_WOIF__Y_Center_Pixel), ZC_GUI_WF__Movable | ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
        bnt__particles_count(ZC_GUI_ButtonNumber<ui_zc>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.particles_count, 1, 10000, 1, 2, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Particles_count, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Particles amount", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  spawn shape
        t__spawn_shape(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Spawn Shape ", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__spawn_shape__shape(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Circle", L"Sphere", L"Square", L"Cube", L"Hemisphere" }, ui_zc(pPS->c_ps_src.spawn_shape.shape), 0.f, 0.f, { &Setup__G_ParticleSystem::Spawn_shape__shape, this }),
        bnt__spawn_shape__fill_to_center(ZC_GUI_ButtonNumber<uch_zc>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0, 0, pPS->c_ps_src.spawn_shape.fill_to_center * 100, 0, 100, 1, 5, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_shape__fill_to_center, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Fill to center %", true, G_GUI_Fonts::Get(G_GUI_FN__Arial_20)->CalculateWstrWidth(wstr_spawn_shape__half_length), ZC_GUI_TextAlignment::Right, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  spawn mat model
        t__spawn_mat_model(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Spawn Mat Model", true, 0, ZC_GUI_TextAlignment::Left),
        t__spawn_mat_model__tanslate(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Translate", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__spawn_mat_model__translate_x(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[0], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__position_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__translate_y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[1], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__position_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__translate_z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[2], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__position_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        t__spawn_mat_model__rotate(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Rotate", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__spawn_mat_model__rotate_order(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"x->y->z", L"x->z->y", L"y->x->z", L"z->x->y", L"y->z->x", L"z->y->x" }, pPS->c_ps_src.spawn_mat_model.rotate_order, 0.f, 0.f, { &Setup__G_ParticleSystem::Spawn_Mat_model__rotate_order, this }),
        bnt__spawn_mat_model__rotate_X(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_X, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__rotate_X, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__rotate_Y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__rotate_Y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__rotate_Z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__rotate_Z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        t__spawn_mat_model__scale(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Scale", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__spawn_mat_model__scale_X(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[0], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__scale_X, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__scale_Y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[1], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__scale_Y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__spawn_mat_model__scale_Z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[2], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Spawn_Mat_model__scale_Z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  size
        t__size(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Size", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__size__width(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.size.width, 0.01f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Size__width, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Width", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__size__height(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.size.height, 0.01f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Size__height, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Height", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  life time
        t__life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Life Time (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__life_time__secs_to_start_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_to_start_max, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Life_time__secs_to_start_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Life start at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__life_time__secs_min(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_min, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Life_time__secs_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Duration min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__life_time__secs_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_max, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Life_time__secs_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Duration max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  move set
        t__move_set(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Move (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        t__move_set__direction_type(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Type (local space)", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__move_set__direction_type(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"From center", L"Direction", L"Point" }, pPS->c_ps_src.move.direction_type, 0.f, 0.f, { &Setup__G_ParticleSystem::Move_set__direction_type, this }),
        bnt__move_set__move_variable_x(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[0], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__move_variable_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__move_set__move_variable_y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[1], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__move_variable_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__move_set__move_variable_z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[2], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__move_variable_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__move_set__speed_power(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_power, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__speed_power, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed power", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__move_set__speed_min(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_min, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__speed_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__move_set__speed_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_max, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Move_set__speed_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  rotate
        t__rotate(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Rotate (degrees)", true, 0, ZC_GUI_TextAlignment::Left),
        t__rotate__angle_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle use", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__rotate__angle_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Constant", L"Random constant", L"Random value" }, pPS->c_ps_src.rotate.angle_use, 0.f, 0.f, { &Setup__G_ParticleSystem::Rotate__angle_use, this }),
        bnt__rotate__angle_1(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.rotate.angle_1, -1000.f, 1000.f, 1.f, 3.f, 1, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Rotate__angle_1, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle 1", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        bnt__rotate__angle_2(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.rotate.angle_2, -1000.f, 1000.f, 1.f, 3.f, 1, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Rotate__angle_2, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle 2", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  animation
        t__animation(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Animation (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
        t__animation__repeat(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Repeat", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__animation__repeat(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Loop", L"Single pass" }, pPS->c_ps_src.animation.repaet, 0.f, 0.f, { &Setup__G_ParticleSystem::Animation__repeat, this }),
        bnt__animation__tiles_per_second(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.animation.tiles_per_second, 0.f, 1000.f, 0.1f, 0.5f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Animation__tiles_per_second, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Tiles per second", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        t__animation__offset_from(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Offset from life", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__animation__offset_from(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Start", L"End" }, pPS->c_ps_src.animation.offset_from, 0.f, 0.f, { &Setup__G_ParticleSystem::Animation__offset_from, this }),
        bnt__animation__secs_offset_to_start_animation(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.animation.offset_to_start_animation_secs, 0.f, 1000.f, 0.1f, 0.5f, 0, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Animation__secs_offset_to_start_animation, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Start animation at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
            //  color
        t__color(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Color", true, 0, ZC_GUI_TextAlignment::Left),
        t__color__rgb_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Use RGB: ", true, 0, ZC_GUI_TextAlignment::Left),
        st__color__rgb_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Add", L"Replace" }, 0.f, 0.f, true, 10.f, { &Setup__G_ParticleSystem::Color__rgb_use, this }, pPS->c_ps_src.color.rgb_use),
        t__color__start(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Start", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__color__appear_secs(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.color.appear_secs, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Color__appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Appear (seconds)", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        cm__color__rgba_start(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { &Setup__G_ParticleSystem::Color__rgba_start, this }, false),
        t__color__end(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"End", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__color__disappear_secs(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.color.disappear_secs, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Color__disappear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Disappear (seconds)", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        cm__color__rgba_end(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { &Setup__G_ParticleSystem::Color__rgba_end, this }, false),
            //  collision
        t__collision(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Collision", true, 0, ZC_GUI_TextAlignment::Left),
        bnt__collision__particle_radius(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_pc_src.particle_radius, 0.f, 100.f, 0.1f, 0.4f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Collision__particle_radius, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Particle radius", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        t__collision__set_life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Jump to life time:", true, 0, ZC_GUI_TextAlignment::Left),
        sdd__collision__set_life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"None", L"Start Animation" }, pPS->c_pc_src.set_life_time, 0.f, 0.f, { &Setup__G_ParticleSystem::Collision__set_life_time, this }),
        bnt__collision__move_speed(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_pc_src.move_speed, -100.f, 100.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &Setup__G_ParticleSystem::Collision__move_speed, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"% Move speed", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0)))
    {
        float section_y = 20;
        float sub_section_y = 15;
        float row_y = 10;
        float distacne_x = 10.f;
        float indent_left = 10.f;
            //  particles count
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__particles_count.GetObj() }));
            //  spawn shape
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_shape.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { sdd__spawn_shape__shape.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_shape__fill_to_center.GetObj() }));
            //  spwn mat model
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__tanslate.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_z.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__rotate.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { sdd__spawn_mat_model__rotate_order.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_X.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_Y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_Z.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__scale.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_X.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_Y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_Z.GetObj() }));
            //  size
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__size.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__size__width.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__size__height.GetObj() }));
            //  life time
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__life_time.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_to_start_max.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_min.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_max.GetObj() }));
            //  move
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__move_set.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_power.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_min.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_max.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, sub_section_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__move_set__direction_type.GetObj(), sdd__move_set__direction_type.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_z.GetObj() }));
        if (pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center)
        {
            bnt__move_set__move_variable_x.SetDrawState(false);
            bnt__move_set__move_variable_y.SetDrawState(false);
            bnt__move_set__move_variable_z.SetDrawState(false);
        }
            //  rotate
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__rotate.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__rotate__angle_use.GetObj(), sdd__rotate__angle_use.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__rotate__angle_1.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__rotate__angle_2.GetObj() }));
        if (pPS->c_ps_src.rotate.angle_use == G_PS_Source::Rotate::AU__contant) bnt__rotate__angle_2.SetDrawState(false);
            //  animation
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__animation.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Bottom), { t__animation__repeat.GetObj(), sdd__animation__repeat.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__animation__tiles_per_second.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__animation__offset_from.GetObj(), sdd__animation__offset_from.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__animation__secs_offset_to_start_animation.GetObj() }));
            //  color
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__rgb_use.GetObj(), st__color__rgb_use.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__start.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__color__appear_secs.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { cm__color__rgba_start.GetObj() }));
        cm__color__rgba_start.SetColor<float>(&(pPS->c_ps_src.color.rgba_start[0]), &(pPS->c_ps_src.color.rgba_start[1]), &(pPS->c_ps_src.color.rgba_start[2]), false);
        cm__color__rgba_start.SetAlpha(pPS->c_ps_src.color.rgba_start[3], false);
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__end.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__color__disappear_secs.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { cm__color__rgba_end.GetObj() }));
        cm__color__rgba_end.SetColor<float>(&(pPS->c_ps_src.color.rgba_end[0]), &(pPS->c_ps_src.color.rgba_end[1]), &(pPS->c_ps_src.color.rgba_end[2]), false);
        cm__color__rgba_end.SetAlpha(pPS->c_ps_src.color.rgba_end[3], false);
            //  collision
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__collision.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__collision__particle_radius.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__collision__set_life_time.GetObj(), sdd__collision__set_life_time.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__collision__move_speed.GetObj() }));
    }
    
    void Particles_count(ui_zc v) { pPS->Set_Particles_count(v); }
        //  spawn shape
    void Spawn_shape__shape(ui_zc v) { pPS->Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape(v)); }
    void Spawn_shape__fill_to_center(uch_zc v) { pPS->Set_SpawnShape__fill_to_center(v / 100.f); }
        //  spawn mat model
    void Spawn_Mat_model__position_x(float v) { pPS->Set_SpawnMatModel__translation({ v, pPS->c_ps_src.spawn_mat_model.translate[1], pPS->c_ps_src.spawn_mat_model.translate[2] }); }
    void Spawn_Mat_model__position_y(float v) { pPS->Set_SpawnMatModel__translation({ pPS->c_ps_src.spawn_mat_model.translate[0], v, pPS->c_ps_src.spawn_mat_model.translate[2] }); }
    void Spawn_Mat_model__position_z(float v) { pPS->Set_SpawnMatModel__translation({ pPS->c_ps_src.spawn_mat_model.translate[0], pPS->c_ps_src.spawn_mat_model.translate[1], v }); }
    void Spawn_Mat_model__rotate_order(ui_zc v) { pPS->Set_SpawnMatModel__rotate(G_PS_Source::SpawnMatModel::RotateOrder(v),
        pPS->c_ps_src.spawn_mat_model.rotate_angle_X, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
    void Spawn_Mat_model__rotate_X(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
        v, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
    void Spawn_Mat_model__rotate_Y(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
        pPS->c_ps_src.spawn_mat_model.rotate_angle_X, v, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
    void Spawn_Mat_model__rotate_Z(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
        pPS->c_ps_src.spawn_mat_model.rotate_angle_X, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, v); }
    void Spawn_Mat_model__scale_X(float v) { pPS->Set_SpawnMatModel__scale({v, pPS->c_ps_src.spawn_mat_model.scale[1], pPS->c_ps_src.spawn_mat_model.scale[2] }); }
    void Spawn_Mat_model__scale_Y(float v) { pPS->Set_SpawnMatModel__scale({pPS->c_ps_src.spawn_mat_model.scale[0], v, pPS->c_ps_src.spawn_mat_model.scale[2] }); }
    void Spawn_Mat_model__scale_Z(float v) { pPS->Set_SpawnMatModel__scale({pPS->c_ps_src.spawn_mat_model.scale[0], pPS->c_ps_src.spawn_mat_model.scale[1], v }); }
        //  size
    void Size__width(float v) { pPS->Set_Size__width(v); }
    void Size__height(float v) { pPS->Set_Size__height(v); }
        //  life time
    void Life_time__secs_to_start_max(float v) { pPS->Set_Life_time__secs_to_start_max(v); }
    void Life_time__secs_min(float v) { pPS->Set_Life_time__min(v); }
    void Life_time__secs_max(float v) { pPS->Set_Life_time__max(v); }
        //  move
    void Move_set__direction_type(ui_zc v)
    {
        bnt__move_set__move_variable_x.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
        bnt__move_set__move_variable_y.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
        bnt__move_set__move_variable_z.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
        pPS->Set_Move__direction_type(G_PS_Source::Move::DirectionType(v));
    }
    void Move_set__move_variable_x(float v) { pPS->Set_Move__move_variable({ v, pPS->c_ps_src.move.variable[1], pPS->c_ps_src.move.variable[2] }); }
    void Move_set__move_variable_y(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.variable[0], v, pPS->c_ps_src.move.variable[2] }); }
    void Move_set__move_variable_z(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.variable[0], pPS->c_ps_src.move.variable[1], v }); }
    void Move_set__speed_power(float v) { pPS->Set_Move__speed_power(v); }
    void Move_set__speed_min(float v) { pPS->Set_Move__speed_min(v); }
    void Move_set__speed_max(float v) { pPS->Set_Move__speed_max(v); }
        //  rotate
    void Rotate__angle_use(ui_zc v)
    {
        bnt__rotate__angle_2.SetDrawState(v != G_PS_Source::Rotate::AU__contant);
        pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = G_PS_Source::Rotate::AngleUse(v), .angle_1 = pPS->c_ps_src.rotate.angle_1, .angle_2 = pPS->c_ps_src.rotate.angle_2 });
    }
    void Rotate__angle_1(float v) { pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = pPS->c_ps_src.rotate.angle_use, .angle_1 = v, .angle_2 = pPS->c_ps_src.rotate.angle_2 }); }
    void Rotate__angle_2(float v) { pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = pPS->c_ps_src.rotate.angle_use, .angle_1 = pPS->c_ps_src.rotate.angle_1, .angle_2 = v, }); }
        //  animation
    void Animation__repeat(ui_zc v) { pPS->Set_Animation__repeat(G_PS_Source::Animation::Repaet(v)); }
    void Animation__tiles_per_second(float v) { pPS->Set_Animation__tiles_per_second(v); }
    void Animation__offset_from(ui_zc v) { pPS->Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom(v)); }
    void Animation__secs_offset_to_start_animation(float v) { pPS->Set_Animation__secs_offset_to_start_animation(v); }
        //  color
    void Color__appear_secs(float v) { pPS->Set_Color__appear_secs(v); }
    void Color__rgb_use(uint v) { pPS->Set_Color__rgb_use(G_PS_Source::Color::RGBUse(v)); }
    void Color__rgba_start(float r, float g, float b, float a) { pPS->Set_Color__rgba_start({ r, g, b, a }); }
    void Color__disappear_secs(float v) { pPS->Set_Color__disappear_secs(v); }
    void Color__rgba_end(float r, float g, float b, float a) { pPS->Set_Color__rgba_end({ r, g, b, a }); }
        //  collision action
    void Collision__particle_radius(float v) { pPS->Set_Collision__particle_radius(v); }
    void Collision__set_life_time(ui_zc v) { pPS->Set_Collision__set_life_time(G_ParticleSystem::Collision::SetLifeTime(v)); }
    void Collision__move_speed(float v) { pPS->Set_Collision__move_speed(v / 100.f); }
};
#endif SETUP__G_PARTICLE_SYSTEM