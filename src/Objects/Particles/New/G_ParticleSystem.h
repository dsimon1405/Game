#pragma once

#include "G_PS_Source.h"

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
    const Collision& c_collision = collision;

    G_ParticleSystem(const G_PS_Source& _ps_source, const Collision& collision);
    ~G_ParticleSystem();

    void SetDrawState(bool need_draw);
        //  return true if drawing, otherwise false
    bool IsDrawing();

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
    void Set_Color__system_alpha(float system_alpha);
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
        float system_alpha = 0.f;   //  visibility of all system
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

    Collision collision;
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