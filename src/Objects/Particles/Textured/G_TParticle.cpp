#include "G_TParticle.h"

#include <ZC/File/ZC_File.h>
#include <System/G_RenderLevel.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <System/G_ShPComputeName.h>

#include <vector>
#include <string>

G_ParticleSystem::G_ParticleSystem(const G_PS_Source& ps_source)
    : ps_src(ps_source),
    ps{CreateParticleSystem()},
    particles(FillParticles()),
    ds(CreateDrawerSet()),
    ds_con(ds.MakeZC_DSController(0))
{
#ifdef SETUP__G_PARTICLE_SYSTEM
    upSP = new Setup__G_ParticleSystem(this);
#endif SETUP__G_PARTICLE_SYSTEM
    
    ec_updater = ZC__Updater::Connect({ &G_ParticleSystem::Callback_Updater, this }, G_UL__game_particles);
}

G_ParticleSystem::~G_ParticleSystem()
{
    ec_updater.Disconnect();
}

void G_ParticleSystem::SetDrawState(bool need_draw)
{
    need_draw ? ds_con.SwitchToDrawLvl(ps_src.render.render_level, ps_src.render.drawer_level) : ds_con.SwitchToDrawLvl(ps_src.render.render_level, ZC_DL_None);
}

bool G_ParticleSystem::GetDrawState()
{
    return ds_con.IsDrawing(ps_src.render.render_level);
}

void G_ParticleSystem::Set_Particles_count(ul_zc count)
{
    if (ps_src.particles_count == count) return;
    ps_src.particles_count = count;

    particles = FillParticles();

    ps.time_total_secs = 0.f;
    ps.time_prev_frame_secs = 0.f;

    ZC_Buffer ssbo_particles(GL_SHADER_STORAGE_BUFFER, G_BIND_SSBO_PARTICLE);
    ssbo_particles.GLNamedBufferStorage(sizeof(G_ParticleSystem::ParticleSystem) + (sizeof(G_ParticleSystem::Particle) * particles.size()), nullptr, GL_DYNAMIC_STORAGE_BIT);
    ssbo_particles.GLNamedBufferSubData(0, sizeof(G_ParticleSystem::ParticleSystem), &ps);
    ssbo_particles.GLNamedBufferSubData(sizeof(G_ParticleSystem::ParticleSystem), sizeof(G_ParticleSystem::Particle) * particles.size(), particles.data());

    bool need_draw = GetDrawState();
    if (need_draw) SetDrawState(false);    //  will be added back to the end of the method to update pointers to ZC_GLDraw, ZC_VAO in ZC_DrawerFL

    ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, count);

        //  ds
    ds.upGLDraw = std::move(upDraw);
    ds.buffers.front() = std::move(ssbo_particles);
    ds.shPCompute.num_groups_x = count;

        //  ds_con
    ds_con.pGLDraw = ds.upGLDraw.Get();

    std::forward_list<ZC_Buffer*> ssbo_buffers{ &(ds.buffers.front()), &*(++(ds.buffers.begin())) };
    ds_con.ssbo_buffers = std::move(ssbo_buffers);

    if (need_draw) SetDrawState(need_draw);
}

void G_ParticleSystem::Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape shape)
{
    if (ps_src.spawn_shape.shape == shape) return;
    ps_src.spawn_shape.shape = shape;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_SpawnShape__fill_to_center(float fill_to_center)
{
    if (ps_src.spawn_shape.fill_to_center == fill_to_center) return;
    ps_src.spawn_shape.fill_to_center = fill_to_center;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_SpawnMatModel__translation(const ZC_Vec3<float>& pos)
{
    if (ps_src.spawn_mat_model.translate == pos) return;
    ps_src.spawn_mat_model.translate = pos;
    ps.spawn_mat_model = CreateSpawnMatModel();
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, spawn_mat_model), sizeof(ParticleSystem::spawn_mat_model), &ps.spawn_mat_model);
}

void G_ParticleSystem::Set_SpawnMatModel__rotate(G_PS_Source::SpawnMatModel::RotateOrder rotate_order, float x, float y, float z)
{
    if (ps_src.spawn_mat_model.rotate_order == rotate_order && ps_src.spawn_mat_model.rotate_angle_X == x
        && ps_src.spawn_mat_model.rotate_angle_Y == y && ps_src.spawn_mat_model.rotate_angle_Z == z) return;
    ps_src.spawn_mat_model.rotate_order = rotate_order;
    ps_src.spawn_mat_model.rotate_angle_X = x;
    ps_src.spawn_mat_model.rotate_angle_Y = y;
    ps_src.spawn_mat_model.rotate_angle_Z = z;
    ps.spawn_mat_model = CreateSpawnMatModel();
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, spawn_mat_model), sizeof(ParticleSystem::spawn_mat_model), &ps.spawn_mat_model);
}

void G_ParticleSystem::Set_SpawnMatModel__scale(const ZC_Vec3<float>& scale)
{
    if (scale == ps_src.spawn_mat_model.scale) return;
    ps_src.spawn_mat_model.scale = scale;
    ps.spawn_mat_model = CreateSpawnMatModel();
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, spawn_mat_model), sizeof(ParticleSystem::spawn_mat_model), &ps.spawn_mat_model);
}

void G_ParticleSystem::Set_Size__width(float width)
{
    if (ps_src.size.width == width) return;
    ps_src.size.width = width;
    ps.size_half_width = width / 2.f;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, size_half_width), sizeof(ParticleSystem::size_half_width), &ps.size_half_width);
}

void G_ParticleSystem::Set_Size__height(float height)
{
    if (ps_src.size.height == height) return;
    ps_src.size.height = height;
    ps.size_half_height = height / 2.f;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, size_half_height), sizeof(ParticleSystem::size_half_height), &ps.size_half_height);
}

void G_ParticleSystem::Set_Life_time__secs_to_start_max(float secs_to_start_max)
{
    if (ps_src.life_time.secs_to_start_max == secs_to_start_max) return;
    ps_src.life_time.secs_to_start_max = secs_to_start_max;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Life_time__min(float secs_min)
{
    if (ps_src.life_time.secs_min == secs_min) return;
    ps_src.life_time.secs_min = secs_min;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Life_time__max(float secs_max)
{
    if (ps_src.life_time.secs_max == secs_max) return;
    ps_src.life_time.secs_max = secs_max;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Visibility__start(float visibility_appear_secs)
{
    if (ps_src.visibility.appear_secs == visibility_appear_secs) return;
    ps_src.visibility.appear_secs = visibility_appear_secs;
    ps.visibility_appear_secs = visibility_appear_secs;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, visibility_appear_secs), sizeof(ParticleSystem::visibility_appear_secs), &ps.visibility_appear_secs);
}

void G_ParticleSystem::Set_Visibility__end(float visibility_disappear_secs)
{
    if (ps_src.visibility.disappear_secs == visibility_disappear_secs) return;
    ps_src.visibility.disappear_secs = visibility_disappear_secs;
    ps.visibility_disappear_secs = visibility_disappear_secs;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, visibility_disappear_secs), sizeof(ParticleSystem::visibility_disappear_secs), &ps.visibility_disappear_secs);
}

void G_ParticleSystem::Set_Move__direction_type(G_PS_Source::Move::DirectionType direction_type)
{
    if (ps_src.move.direction_type == direction_type) return;
    ps_src.move.direction_type = direction_type;
    ps.move_direction_type = direction_type;
    ps.move_variable = direction_type == G_PS_Source::Move::DT__variable_is_direction ? ZC_Vec::Normalize(ps_src.move.variable) : ps_src.move.variable;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, move_direction_type),
        sizeof(ParticleSystem::move_direction_type) + sizeof(ParticleSystem::move_variable), &ps.move_direction_type);
}

void G_ParticleSystem::Set_Move__move_variable(const ZC_Vec3<float>& variable)
{
    if (ps_src.move.variable == variable) return;
    ps_src.move.variable = variable;
    ps.move_variable = ps_src.move.direction_type == G_PS_Source::Move::DT__variable_is_direction ? ZC_Vec::Normalize(variable) : variable;  //  make normalization for DT__variable_is_direction once only aon CPU
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, move_variable), sizeof(ParticleSystem::move_variable), &ps.move_variable);
}

void G_ParticleSystem::Set_Move__speed_power(float speed_power)
{
    if (ps_src.move.speed_power == speed_power) return;
    ps_src.move.speed_power = speed_power;
    ps.move_speed_power = speed_power;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, move_speed_power), sizeof(ParticleSystem::move_speed_power), &ps.move_speed_power);
}

void G_ParticleSystem::Set_Move__speed_min(float speed_min_secs)
{
    if (ps_src.move.speed_min == speed_min_secs) return;
    ps_src.move.speed_min = speed_min_secs;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Move__speed_max(float speed_max_secs)
{
    if (ps_src.move.speed_max == speed_max_secs) return;
    ps_src.move.speed_max = speed_max_secs;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Rotate(const G_PS_Source::Rotate& rotate)
{
    if (ps_src.rotate == rotate) return;
    ps_src.rotate = rotate;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Animation__repeat(G_PS_Source::Animation::Repaet repaet)
{
    if (ps_src.animation.repaet == repaet) return;
    ps_src.animation.repaet = repaet;
    ps.animation_repeat = repaet;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Animation__tiles_per_second(float tiles_per_second)
{
    if (ps_src.animation.tiles_per_second == tiles_per_second) return;
    ps_src.animation.tiles_per_second = tiles_per_second;
    ps.animation_uv_shift_speed = 1.f / tiles_per_second;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, animation_uv_shift_speed), sizeof(ParticleSystem::animation_uv_shift_speed), &ps.animation_uv_shift_speed);
}

void G_ParticleSystem::Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom offset_from)
{
    if (ps_src.animation.offset_from == offset_from) return;
    ps_src.animation.offset_from = offset_from;
    SetSpawnDataToDefault();
}

void G_ParticleSystem::Set_Animation__secs_offset_to_start_animation(float offset_to_start_animation_secs)
{
    if (ps_src.animation.offset_to_start_animation_secs == offset_to_start_animation_secs) return;
    ps_src.animation.offset_to_start_animation_secs = offset_to_start_animation_secs;
    SetSpawnDataToDefault();
}

float G_ParticleSystem::GetRandom(float secs_min, float secs_max)
{
    return f_zc(ZC_Random::GetRandomInt(secs_min * f_100, secs_max * f_100)) / f_100;
}

ZC_Mat4<f_zc> G_ParticleSystem::CreateSpawnMatModel()
{
    ZC_Mat4<float> spawn_mat_model(1.f);
        //  translate
    spawn_mat_model.Translate(ps_src.spawn_mat_model.translate);
        //  rotate
    switch (ps_src.spawn_mat_model.rotate_order)
    {
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_xyz:   //  reverse call order
    {
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
    } break;
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_xzy:   //  reverse call order
    {
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
    } break;
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_yxz:   //  reverse call order
    {
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
    } break;
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_zxy:   //  reverse call order
    {
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
    } break;
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_yzx:
    {
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
    } break;
    case G_PS_Source::SpawnMatModel::RotateOrder::RO_zyx:
    {
        if (ps_src.spawn_mat_model.rotate_angle_X != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_X, { 1.f, 0.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Y != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Y, { 0.f, 1.f, 0.f });
        if (ps_src.spawn_mat_model.rotate_angle_Z != 0.f) spawn_mat_model.Rotate(ps_src.spawn_mat_model.rotate_angle_Z, { 0.f, 0.f, 1.f });
    } break;
    }
        //  scale
    spawn_mat_model.Scale(ps_src.spawn_mat_model.scale);
    return spawn_mat_model;
}

G_ParticleSystem::ParticleSystem G_ParticleSystem::CreateParticleSystem()
{
    return ParticleSystem
    {
            //  time
        // .time_prev_frame_secs = ,     //  sets in Update
        // .time_total_secs = ,          //  sets in Update
            //  pos
        .spawn_mat_model = CreateSpawnMatModel(),
            //  texture size
        .size_half_width = ps_src.size.width / 2.f,
        .size_half_height = ps_src.size.height / 2.f,
            //  visibility
        .visibility_appear_secs = ps_src.visibility.appear_secs,
        .visibility_disappear_secs = ps_src.visibility.disappear_secs,
            //  mpve
        .move_direction_type = ps_src.move.direction_type,
        .move_variable = ps_src.move.direction_type == G_PS_Source::Move::DT__variable_is_direction ? ZC_Vec::Normalize(ps_src.move.variable) : ps_src.move.variable,   //  in this case there is no need to calculate on the GPU, it is calculated once on the CPU
        .move_speed_power = ps_src.move.speed_power,
            //  animation
        .animation_repeat = ps_src.animation.repaet,
        .animation_uv_shift_speed = ps_src.animation.tiles_per_second == 0.f ? 0.f : 1.f / ps_src.animation.tiles_per_second,
    };
}

std::vector<G_ParticleSystem::Particle> G_ParticleSystem::FillParticles()
{
    std::vector<Particle> particles_temp;
    particles_temp.reserve(ps_src.particles_count);

    for (ul_zc i = 0; i < particles_temp.capacity(); ++i)
    {
        float life_time_secs_total = ps_src.life_time.secs_min == ps_src.life_time.secs_max ? ps_src.life_time.secs_min : GetRandom(ps_src.life_time.secs_min, ps_src.life_time.secs_max);
        particles_temp.emplace_back(Particle
            {
                    //  life time
                .life_time_secs_to_start = ps_src.life_time.secs_to_start_max == 0.f ? 0.f : GetRandom(0.f, ps_src.life_time.secs_to_start_max),
                .life_time_secs_total = life_time_secs_total,
                // .life_time_secs_cur          //  calc on gpu
                    //  pos
                .pos_start = {},
                // .pos_cur                     //  calc on gpu
                    //  visibility
                // .visibility_alpha            //  calc on gpu
                    //  move
                // .move_dir_normalized         //  calc on gpu
                .move_speed_secs = ps_src.move.speed_min == ps_src.move.speed_max ? ps_src.move.speed_min : G_ParticleSystem::GetRandom(ps_src.move.speed_min, ps_src.move.speed_max),
                    //  texture corners rotated frace to cam
                // .size_bl                 //  calc on gpu
                // .size_br                 //  calc on gpu
                // .size_tl                 //  calc on gpu
                // .size_tr                 //  calc on gpu
                .rotate_angle = ps_src.rotate.GetAngle(),
                    //  animaion
                .animation_start_secs = ps_src.animation.Calc_animation_start_secs(life_time_secs_total),     //  when in life time to start animation
                // uint animation_uvs_cur_id           //  calc on gpu
                // uint animation_uvs_cur_id_secs      //  calc on gpu
            });
    }
        

    FillParticlesPosition(particles_temp);

    return particles_temp;
}

void G_ParticleSystem::FillParticlesPosition(std::vector<G_ParticleSystem::Particle>& rParticles)
{
    switch (ps_src.spawn_shape.shape)
    {
    case G_PS_Source::SpawnShape::S__Circle: FillShapeCircle(rParticles); break;
    case G_PS_Source::SpawnShape::S__Sphere: FillShapeSphere(rParticles, false); break;
    case G_PS_Source::SpawnShape::S__Cube: FillShapeCube(rParticles); break;
    case G_PS_Source::SpawnShape::S__Square: FillShapeSquare(rParticles); break;
    case G_PS_Source::SpawnShape::S__Hemisphere: FillShapeSphere(rParticles, true); break;
    }
}

void G_ParticleSystem::SetAxisQuaterSigne2D(ZC_Vec3<float>& pos_xy)
{
    enum AxisQuater
    {                       //  If +x to right, +y to up
        x_neg__y_pos,       //  tl
        x_pos__y_pos,       //  tr
        x_pos__y_neg,       //  br
        x_neg__y_neg        //  bl
    } static last_quater = x_neg__y_pos;
    switch (last_quater)  //  each new particle have pos at the next quater of the shape with clockwise
    {
    case x_neg__y_pos: pos_xy[0] *= -1.f; break;
    case x_pos__y_pos: break;   //  default values are positive
    case x_pos__y_neg: pos_xy[1] *= -1.f; break;
    case x_neg__y_neg: { pos_xy[0] *= -1.f, pos_xy[1]*= -1.f; } break;
    }
    last_quater = AxisQuater(last_quater + 1);
    if (last_quater > x_neg__y_neg) last_quater = x_neg__y_pos;
}

void G_ParticleSystem::SetAxisQuaterSigne3D(ZC_Vec3<float>& pos_xyz)
{
    enum AxisEighth
    {                           //  Look from -y: +x to right, +y to front, +z to up
        x_neg__y_pos__z_pos,    //  tl far
        x_pos__y_pos__z_pos,    //  tr far
        x_pos__y_neg__z_pos,    //  tr near
        x_neg__y_neg__z_pos,    //  tl near
        x_neg__y_pos__z_neg,    //  bl far
        x_pos__y_pos__z_neg,    //  br far
        x_pos__y_neg__z_neg,    //  br near
        x_neg__y_neg__z_neg,    //  bl near
    } static last_eighth = x_neg__y_pos__z_pos;
    switch (last_eighth)  //  each new particle have pos at the next eighth of the shape with clockwise, first far then near
    {
    case x_neg__y_pos__z_pos: pos_xyz[0] *= -1.f; break;
    case x_pos__y_pos__z_pos: break;   //  default values are positive
    case x_pos__y_neg__z_pos: pos_xyz[1] *= -1.f; break;
    case x_neg__y_neg__z_pos: { pos_xyz[0] *= -1.f; pos_xyz[1] *= -1.f; } break;
    case x_neg__y_pos__z_neg: { pos_xyz[0] *= -1.f; pos_xyz[2] *= -1.f; } ; break;
    case x_pos__y_pos__z_neg: pos_xyz[2] *= -1.f; break;
    case x_pos__y_neg__z_neg: { pos_xyz[1] *= -1.f; pos_xyz[2] *= -1.f; } ; break;
    case x_neg__y_neg__z_neg: pos_xyz *= -1.f; break;
    }
    last_eighth = AxisEighth(last_eighth + 1);
    if (last_eighth > x_neg__y_neg__z_neg) last_eighth = x_neg__y_pos__z_pos;
}

void G_ParticleSystem::FillShapeCircle(std::vector<G_ParticleSystem::Particle>& rParticles)
{
    float radius_min = radius_or_half_length_max - (radius_or_half_length_max * ps_src.spawn_shape.fill_to_center);

    for (Particle& p : rParticles)
    {
        float dir_x = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
        float dir_y = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
        ZC_Vec2<float> pos_xy = ZC_Vec::MoveByLength({}, { dir_x, dir_y }, radius_min != radius_or_half_length_max ? GetRandom(radius_min, radius_or_half_length_max) : radius_or_half_length_max);
        p.pos_start = ZC_Vec3<float>(pos_xy[0], pos_xy[1], 0.f);
        SetAxisQuaterSigne2D(p.pos_start);
    }
}

void G_ParticleSystem::FillShapeSphere(std::vector<Particle>& rParticles, bool hemisphere)
{
    float radius_min = radius_or_half_length_max - (radius_or_half_length_max * ps_src.spawn_shape.fill_to_center);

    float rot_angle = ZC_angle_360f / std::floor(std::sqrt(ps_src.particles_count));   //  round to integer count of particles in mesh
        //  random way
    for (Particle& p : rParticles)
    {
        float rotate_z = GetRandom(0.f, 90.f);
        float rotate_x = GetRandom(0.f, 90.f);
        ZC_Mat4<float> model(1.f);
        if (rotate_x != 0.f) model.Rotate(rotate_x, { 1.f, 0.f, 0.f});
        if (rotate_z != 0.f) model.Rotate(rotate_z, { 0.f, 0.f, -1.f});
        ZC_Vec3<float> pos = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, radius_min != radius_or_half_length_max ? float(GetRandom(radius_min, radius_or_half_length_max)) : radius_min, 0.f, 1.f));
        hemisphere ? SetAxisQuaterSigne2D(pos) : SetAxisQuaterSigne3D(pos);     //  if hemisphere, fill only Z positive quaters
        p.pos_start = pos;
    }

        //  calculate way, not good
    // ul_zc rParticles_i = 0;
    // for (float cur_angle_X = 0.f; cur_angle_X < ZC_angle_360f; cur_angle_X = cur_angle_X + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_X + rot_angle)
    // {
    //     for (float cur_angle_Z = 0.f; cur_angle_Z < ZC_angle_360f; cur_angle_Z = cur_angle_Z + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_Z + rot_angle, ++rParticles_i)
    //     {       //  calculate position
    //         ZC_Mat4<float> model(1.f);
    //         if (cur_angle_X != 0.f) model.Rotate(cur_angle_X, { 1.f, 0.f, 0.f});
    //         if (cur_angle_Z != 0.f) model.Rotate(cur_angle_Z, { 0.f, 0.f, 1.f});
    //         ZC_Vec3<float> pos = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, radius_min != radius_or_half_length_max ? float(GetRandom(radius_min, radius_or_half_length_max)) : radius_min, 0.f, 1.f));
    //         rParticles[rParticles_i].pos_start = pos;
    //     }
    // }

    // size_t rest_sphere_particles = rParticles.size() - rParticles_i;
    // for (; rParticles_i < rest_sphere_particles; ++rParticles_i)  //  sphere mesh may not be edeal, so rest particles add with random rotation
    // {
    //     ZC_Vec3<float> pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4(1.f).Rotate(ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i), {1.f, 1.f, 1.f})
    //         * ZC_Vec4<float>(0.f, radius_min != radius_or_half_length_max ? float(GetRandom(radius_min, radius_or_half_length_max)) : radius_min, 0.f, 1.f));
    //     rParticles[rParticles_i].pos_start = pos;
    // }
}

void G_ParticleSystem::FillShapeSquare(std::vector<Particle>& rParticles)
{
    float half_length_min = radius_or_half_length_max - (radius_or_half_length_max * ps_src.spawn_shape.fill_to_center);
    
    enum ZeroHalfLengthMin  //  to have empty space into the shape equal to half length min need to use random from zero for one of the axises X or Y
    {
        ZHLM_x,
        ZHLM_y
    } static zhlm = ZHLM_x;
    
    i_zc quater_counter = 0;
    for (Particle& p : rParticles)
    {
        float pos_x = GetRandom(zhlm == ZHLM_x ? 0.f : half_length_min, radius_or_half_length_max);
        float pos_y = GetRandom(zhlm == ZHLM_y ? 0.f : half_length_min, radius_or_half_length_max);
        p.pos_start = ZC_Vec3<float>(pos_x, pos_y, 0.f);
        SetAxisQuaterSigne2D(p.pos_start);

        if (++quater_counter == 4)     //  change random state for half_length_min each full-filled round of quaters
        {
            zhlm = zhlm == ZHLM_x ? ZHLM_y : ZHLM_x;
            quater_counter = 0;
        }
    }
}
    
void G_ParticleSystem::FillShapeCube(std::vector<Particle>& rParticles)
{
    float half_length_min = radius_or_half_length_max - (radius_or_half_length_max * ps_src.spawn_shape.fill_to_center);
    
    enum UseHalfLengthMin  //  to have empty space into the shape equal to half length min need to use random from zero for one of the axises X or Y or Z
    {
        UHLM_x,
        UHLM_y,
        UHLM_z
    } static uhlm = UHLM_x;

    i_zc eighth_counter = 0;
    for (Particle& p : rParticles)
    {
        float pos_x = GetRandom(uhlm == UHLM_x ? half_length_min : 0.f, radius_or_half_length_max);
        float pos_y = GetRandom(uhlm == UHLM_y ? half_length_min : 0.f, radius_or_half_length_max);
        float pos_z = GetRandom(uhlm == UHLM_z ? half_length_min : 0.f, radius_or_half_length_max);
        p.pos_start = ZC_Vec3<float>(pos_x, pos_y, pos_z);
        SetAxisQuaterSigne3D(p.pos_start);

        if (++eighth_counter == 8)     //  change random state for half_length_min each full-filled round of the eighth.
        {
            uhlm = UseHalfLengthMin(uhlm + 1);
            if (uhlm > UHLM_z) uhlm = UHLM_x;
            eighth_counter = 0;
        }
    }
}

// #include <iostream>
ZC_DrawerSet G_ParticleSystem::CreateDrawerSet()
{
    ZC_Buffer ssbo_particles(GL_SHADER_STORAGE_BUFFER, G_BIND_SSBO_PARTICLE);
    ssbo_particles.GLNamedBufferStorage(sizeof(ParticleSystem) + (sizeof(Particle) * particles.size()), nullptr, GL_DYNAMIC_STORAGE_BIT);
    ssbo_particles.GLNamedBufferSubData(0, sizeof(ParticleSystem), &ps);
    ssbo_particles.GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());

        //  texture
    ZC_Texture tex = ZC_Texture::LoadTexture2D(ps_src.render.tex_path.c_str(), 0, GL_REPEAT, GL_REPEAT);
        //  fill tex uv
    const int tile_width = tex.GetWidth() / ps_src.render.columns_count;
    const int tile_heihgt = tex.GetHeight() / ps_src.render.rows_count;
    uint uvs_count = ps_src.render.rows_count * ps_src.render.columns_count;
    std::vector<UV> uvs;
    uvs.reserve(uvs_count);
    for (int row_i = 0, tile_i = 0; row_i < ps_src.render.rows_count; ++row_i)
    {
        for (int column_i = 0; column_i < ps_src.render.columns_count; ++column_i)
        {       //  fill uv from top left corner of the texture to bottom right
            float left_x = column_i * tile_width;
            float top_y = tex.GetHeight() - (row_i * tile_heihgt);  //  opengl draw textures from bottom to top
            uvs.emplace_back(UV
                {
                    .left_x = left_x / tex.GetWidth(),
                    .top_y = top_y / tex.GetHeight(),
                    .right_x = (left_x + tile_width) / tex.GetWidth(),
                    .bottom_y = (top_y - tile_heihgt) / tex.GetHeight(),
                });
        }
    }
    ZC_Buffer ssbo_uvs(GL_SHADER_STORAGE_BUFFER, G_BIND_SSBO_TEX_DATA);
    ssbo_uvs.GLNamedBufferStorage(sizeof(uvs_count) + (sizeof(UV) * uvs.size()), &uvs_count, GL_DYNAMIC_STORAGE_BIT);
    ssbo_uvs.GLNamedBufferSubData(sizeof(uvs_count), sizeof(UV) * uvs.size(), uvs.data());

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, particles.size());

    ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

	ZC_VAO vao;     //  default vao

	std::forward_list<ZC_Buffer> buffers;
    buffers.emplace_front(std::move(ssbo_uvs));
	buffers.emplace_front(std::move(ssbo_particles));

    // ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
    std::vector<ZC_Texture> texs;
    texs.emplace_back(std::move(tex));
	std::forward_list<ZC_TexturesSet> tex_sets;
    tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = std::move(texs) });

    ZC_ShPCompute shPCompute(G_ShPCN__flame, GL_SHADER_STORAGE_BARRIER_BIT, particles.size(), 1, 1);

    return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets), { { ZC_RL_Default } }, shPCompute);
}


void G_ParticleSystem::SetSpawnDataToDefault()
{
    ps.time_prev_frame_secs = 0.f;
    ps.time_total_secs = 0.f;
    ds.buffers.front().GLNamedBufferSubData(0, sizeof(ParticleSystem), &ps);

    particles = FillParticles();
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

#define G_TParticle_Callback_Updater
#ifdef G_TParticle_Callback_Updater
#include <ZC/Tools/Time/ZC_Timer.h>
#include <iostream>
#endif
void G_ParticleSystem::Callback_Updater(float time)
{
    // static std::vector<float> cur_life_secs(pos_and_life_time.size(), 0.f);
    // static std::vector<ZC_Vec4<float>> pos_and_life_time_original = pos_and_life_time;
    // for (size_t i = 0; i < cur_life_secs.size(); ++i)
    // {
    //     float& cur_life_sec = cur_life_secs[i];
    //     cur_life_sec += time;
    //     float life_secs = pos_and_life_time[i][3];
    //     if (cur_life_sec > life_secs) cur_life_sec -= life_secs;
    // }

    // static ZC_Clock cl;
#ifdef G_TParticle_Callback_Updater
    static ZC_Timer timer(ZC_TR__repeats, 5000., ZC_TRO__average, "updt flame");

    timer.StartPoint();

    // ds_con.SwitchToDrawLvl(ps_src.render.render_level, ps_src.render.drawer_level);

    // static const float rot_speed_XY = 60.f;
    // static const float rot_speed_Z = 3.f;
    // static float cur_angle_XY = 0.f;
    // static float cur_angle_Z = 0.f;
    // static float dist_to_center = 142;
    // cur_angle_XY += rot_speed_XY * time;
    // cur_angle_Z += rot_speed_Z * time;
    // ZC_Mat4<float> model = ZC_Mat4<float>(1.f).Rotate(cur_angle_Z, {0.f, 0.f, 1.f}).Translate({ 0.f, dist_to_center, 10.f })
    //     .Rotate(cur_angle_XY, { 1.f, 1.f, 0.f });
    // ZC_Vec3<float> star_center = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, 0.f, 0.f, 1.f));
    // Set_SpawnMatModel__translation(star_center);


    ps.time_prev_frame_secs = time;
    ps.time_total_secs += time;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, time_prev_frame_secs), sizeof(ps.time_prev_frame_secs) + sizeof(ps.time_total_secs), &ps.time_prev_frame_secs);

        //  update time to uniform
    // G_ParticleSystem::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds

    timer.EndPoint();

    //     //  update pos personal
    // for (size_t i = 0ull; i < pos_and_life_time.size(); ++i)
    // {
    //     const ZC_Vec3<float> start_pos = ZC_Vec::Vec4_to_Vec3(pos_and_life_time_original[i]);
    //     const ZC_Vec3<float> move_to(unData[2][0], unData[2][1], unData[2][2]);
    //     ZC_Vec3<float> dir_pos_to_move_to = move_to - start_pos;
    //     const float move_speed_seconds = unData[2][3];
    //     float dist_pos_to_move_to = move_speed_seconds * cur_life_secs[i];
    //     ZC_Vec3<float> pos = ZC_Vec::MoveByLength(start_pos, dir_pos_to_move_to, dist_pos_to_move_to);
    //     ZC_Vec4<float>& pos_and_lt = pos_and_life_time[i];
    //     pos_and_lt[0] = pos[0];
    //     pos_and_lt[1] = pos[1];
    //     pos_and_lt[2] = pos[2];
    // }
    // ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * pos_and_life_time.size(), pos_and_life_time.data());
    
#else
    ps.time_prev_frame_secs = time;
    ps.time_total_secs += time;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, time_prev_frame_secs), sizeof(ps.time_prev_frame_secs) + sizeof(ps.time_total_secs), &ps);
    // G_ParticleSystem::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds
#endif
}

//  sum (cpu + gpu) 1000 particles
//  0.000064 - 0.00011     update time to uniform (power off)
//  0.000147     update pos at cpu
//  0.000065 - 0.000135      ssbo, update time at cpu, all data particle data updates at vertex shader



    //  G_PS_Source::Rotate
#include <ZC/Tools/ZC_Random.h> 

bool G_PS_Source::Rotate::operator == (const Rotate& r) const noexcept
{
    return angle_use == r.angle_use && angle_1 == r.angle_1 && angle_2 == r.angle_2;
}

float G_PS_Source::Rotate::GetAngle()
{
    switch (angle_use)
    {
    case AU__contant: return angle_1;
    case AU__random_between_constants: return ZC_Random::GetRandomInt(0, 1) ? angle_1 : angle_2;
    case AU__random_between_angles: return ZC_Random::GetRandomFloat_x_100(angle_1, angle_2);
    }
    return 0.f;
}


    //  G_PS_Source::Animation

float G_PS_Source::Animation::Calc_animation_start_secs(float life_time_secs_total)
{
    return offset_from == G_PS_Source::Animation::OF__Start ? offset_to_start_animation_secs : life_time_secs_total - offset_to_start_animation_secs;
}