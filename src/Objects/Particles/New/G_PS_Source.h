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
            //  texture
        std::string tex_path;   //  fool path to the texture
        bool create_apha_from_rgb = false;
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
            DT__from_particles_center    = 0,    //  Default direction, ignore Move::variable. On gpu use the normalized Particle::pos_start (particles local space direction) as the direction of movement. If G_PLS__world_space is used, when a particle spawns, local space direction will rotate (just rotate!) with ParticleSystem::spawn_mat_model to have the actual direction at start.
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

        float system_alpha = 1.f;       //  alpha of all system
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