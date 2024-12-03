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
    
    ds_con.SwitchToDrawLvl(ps_src.render.render_level, ps_src.render.drawer_level);

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

    std::vector<G_ParticleSystem::Particle> particles = FillParticles();

    ps.total_secs = 0.f;
    ps.prev_frame_secs = 0.f;

    ZC_Buffer ssbo_particles(GL_SHADER_STORAGE_BUFFER, G_BIND_SSBO_PARTICLE);
    ssbo_particles.GLNamedBufferStorage(sizeof(G_ParticleSystem::ParticleSystem) + (sizeof(G_ParticleSystem::Particle) * particles.size()), nullptr, GL_DYNAMIC_STORAGE_BIT);
    ssbo_particles.GLNamedBufferSubData(0, sizeof(G_ParticleSystem::ParticleSystem), &ps);
    ssbo_particles.GLNamedBufferSubData(sizeof(G_ParticleSystem::ParticleSystem), sizeof(G_ParticleSystem::Particle) * particles.size(), particles.data());

    bool need_draw = GetDrawState();
    if (need_draw) SetDrawState(false);    //  will be added back to the end of the method to update pointers to ZC_GLDraw, ZC_VAO in ZC_DrawerFL

    ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, count);

    particles = std::move(particles);

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

void G_ParticleSystem::Set_Life_space(G_PS_Source::LifeSpace life_space)
{
    if (ps_src.life_space == life_space) return;
    ps_src.life_space = life_space;
    ps.life_space = life_space;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, life_space), sizeof(ParticleSystem::life_space), &ps.life_space);
}

void G_ParticleSystem::Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape shape)
{
    if (ps_src.spawn_shape.shape == shape) return;
    ps_src.spawn_shape.shape = shape;
    FillParticlesPosition(particles);
}

void G_ParticleSystem::Set_SpawnShape__variable_1(float val_1)
{
    if (ps_src.spawn_shape.val_1 == val_1) return;
    ps_src.spawn_shape.val_1 = val_1;
    FillParticlesPosition(particles);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_SpawnShape__variable_2(float val_2)
{
    if (ps_src.spawn_shape.val_2 == val_2) return;
    ps_src.spawn_shape.val_2 = val_2;
    FillParticlesPosition(particles);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_SpawnShape__variable_3(float val_3)
{
    if (ps_src.spawn_shape.val_3 == val_3) return;
    ps_src.spawn_shape.val_3 = val_3;
    if (ps_src.spawn_shape.shape == G_PS_Source::SpawnShape::S__Cube)   //  vall 3 uses only in S__Cube
    {
        FillParticlesPosition(particles);
        ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
    }
}

void G_ParticleSystem::Set_SpawnMatModel__position(const ZC_Vec3<float>& pos)
{
    if (ps_src.spawn_mat_model.position == pos) return;
    ps_src.spawn_mat_model.position = pos;
    ps.mat_model = CreateSpawnMatModel();
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, mat_model), sizeof(ParticleSystem::mat_model), &ps.mat_model);
}

void G_ParticleSystem::Set_SpawnMatModel__rotation(float angle, const ZC_Vec3<float>& axises)
{
    if (ps_src.spawn_mat_model.rotation_angle == angle && ps_src.spawn_mat_model.rotation_axis_power == axises) return;
    ps_src.spawn_mat_model.rotation_angle = angle;
    ps_src.spawn_mat_model.rotation_axis_power = axises;
    ps.mat_model = CreateSpawnMatModel();
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, mat_model), sizeof(ParticleSystem::mat_model), &ps.mat_model);
}

void G_ParticleSystem::Set_Size__widht(float width)
{
    if (ps_src.size.width == width) return;
    ps_src.size.width = width;
    ps.half_width = width / 2.f;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, half_width), sizeof(ParticleSystem::half_width), &ps.half_width);
}

void G_ParticleSystem::Set_Size__height(float height)
{
    if (ps_src.size.height == height) return;
    ps_src.size.height = height;
    ps.half_height = height / 2.f;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, half_height), sizeof(ParticleSystem::half_height), &ps.half_height);
}

void G_ParticleSystem::Set_Life_time__secs_to_start_max(float secs_to_start_max)
{
    if (ps_src.life_time.secs_to_start_max == secs_to_start_max) return;
    ps_src.life_time.secs_to_start_max = secs_to_start_max;
    for (auto& p : particles)
    {
        p.secs_to_start = G_ParticleSystem::GetRandom(0.f, ps_src.life_time.secs_to_start_max);
        p.life_secs_cur = 0.f;
    }
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());

    ps.prev_frame_secs = 0.f;
    ps.total_secs = 0.f;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, prev_frame_secs), sizeof(ParticleSystem::prev_frame_secs) + sizeof(ParticleSystem::total_secs), &ps.prev_frame_secs);
}

void G_ParticleSystem::Set_Life_time__min(float secs_min)
{
    if (ps_src.life_time.secs_min == secs_min) return;
    ps_src.life_time.secs_min = secs_min;
    for (Particle& p : particles) p.life_secs_total = G_ParticleSystem::GetRandom(ps_src.life_time.secs_min, ps_src.life_time.secs_max);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_Life_time__max(float secs_max)
{
    if (ps_src.life_time.secs_max == secs_max) return;
    ps_src.life_time.secs_max = secs_max;
    for (Particle& p : particles) p.life_secs_total = G_ParticleSystem::GetRandom(ps_src.life_time.secs_min, ps_src.life_time.secs_max);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_Visibility__start(float appear_secs)
{
    if (ps_src.visibility.appear_secs == appear_secs) return;
    ps_src.visibility.appear_secs = appear_secs;
    ps.appear_secs = appear_secs;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, appear_secs), sizeof(ParticleSystem::appear_secs), &ps.appear_secs);
}

void G_ParticleSystem::Set_Visibility__end(float disappear_secs)
{
    if (ps_src.visibility.disappear_secs == disappear_secs) return;
    ps_src.visibility.disappear_secs = disappear_secs;
    ps.disappear_secs = disappear_secs;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, disappear_secs), sizeof(ParticleSystem::disappear_secs), &ps.disappear_secs);
}

void G_ParticleSystem::Set_Move__direction_type(G_PS_Source::Move::DirectionType direction_type)
{

}

void G_ParticleSystem::Set_Move__move_variable(const ZC_Vec3<float>& move_variable)
{

}

void G_ParticleSystem::Set_Move__speed_power(float speed_power)
{
    if (ps_src.move.speed_power == speed_power) return;
    ps_src.move.speed_power = speed_power;
    ps.speed_power = speed_power;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, speed_power), sizeof(ParticleSystem::speed_power), &ps.speed_power);
}

void G_ParticleSystem::Set_Move__speed_min(float speed_min_secs)
{
    if (ps_src.move.speed_min == speed_min_secs) return;
    ps_src.move.speed_min = speed_min_secs;
    for (auto& p : particles) p.move_speed_secs = G_ParticleSystem::GetRandom(ps_src.move.speed_min, ps_src.move.speed_max);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_Move__speed_max(float speed_max_secs)
{
    if (ps_src.move.speed_max == speed_max_secs) return;
    ps_src.move.speed_max = speed_max_secs;
    for (auto& p : particles) p.move_speed_secs = G_ParticleSystem::GetRandom(ps_src.move.speed_min, ps_src.move.speed_max);
    ds.buffers.front().GLNamedBufferSubData(sizeof(ParticleSystem), sizeof(Particle) * particles.size(), particles.data());
}

void G_ParticleSystem::Set_Animation__change_tyles_style(G_PS_Source::Animation::ChangeTilesStyle change_tyles_style)
{

}

void G_ParticleSystem::Set_Animation__tiles_per_second(float tiles_per_second)
{
    if (ps_src.animation.tiles_per_second == tiles_per_second) return;
    ps_src.animation.tiles_per_second = tiles_per_second;
    ps.uv_shift_speed = 1.f / tiles_per_second;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, uv_shift_speed), sizeof(ParticleSystem::uv_shift_speed), &ps.uv_shift_speed);
}

void G_ParticleSystem::Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom offset_from)
{

}

void G_ParticleSystem::Set_Animation__secs_offset_to_start_animation(float secs_offset_to_start_animatin)
{

}

float G_ParticleSystem::GetRandom(float secs_min, float secs_max)
{
    return f_zc(ZC_Random::GetRandomInt(secs_min * f_100, secs_max * f_100)) / f_100;
}

ZC_Mat4<f_zc> G_ParticleSystem::CreateSpawnMatModel()
{
    ZC_Mat4<float> mat_model(1.f);
    mat_model.Translate(ps_src.spawn_mat_model.position);
    if (ps_src.spawn_mat_model.rotation_angle != 0.f && (ps_src.spawn_mat_model.rotation_axis_power != ZC_Vec3<float>()))   //  use only if not null value in angle and minimun one of axes
        mat_model.Rotate(ps_src.spawn_mat_model.rotation_angle, ps_src.spawn_mat_model.rotation_axis_power);
    return mat_model;
}

G_ParticleSystem::ParticleSystem G_ParticleSystem::CreateParticleSystem()
{
    return ParticleSystem
    {
        // .prev_frame_secs = ,     //  calc on gpu
        // .total_secs = ,          //  calc on gpu
        .mat_model = CreateSpawnMatModel(),
        // .bl = ,                  //  calc on gpu
        // .br = ,                  //  calc on gpu
        // .tl = ,                  //  calc on gpu
        // .tr = ,                  //  calc on gpu
        .half_width = ps_src.size.width / 2.f,
        .half_height = ps_src.size.height / 2.f,
        .appear_secs = ps_src.visibility.appear_secs,
        .disappear_secs = ps_src.visibility.disappear_secs,
        .life_space = ps_src.life_space,
        .speed_power = ps_src.move.speed_power,
        .uv_shift_speed = 1.f / ps_src.animation.tiles_per_second,
    };
}

void G_ParticleSystem::FillParticlePosition(G_ParticleSystem::Particle& rParticle, const ZC_Vec3<float>& pos_start)
{
    ZC_Vec3<float> pos_cur = pos_start;
    if (ps_src.life_space == G_PS_Source::LifeSpace::LS__world)    //  move to world space
        pos_cur = ZC_Vec::Vec4_to_Vec3(ps.mat_model * ZC_Vec4<float>(pos_start, 1.f));
    rParticle.pos_cur = pos_cur;
    rParticle.pos_start = pos_start;
}

std::vector<G_ParticleSystem::Particle> G_ParticleSystem::FillParticles()
{
    std::vector<Particle> particles_temp;
    particles_temp.reserve(ps_src.particles_count);

    for (ul_zc i = 0; i < particles_temp.capacity(); ++i)
    {
        particles_temp.emplace_back(Particle
            {
                .life_secs_total = ps_src.life_time.secs_min == ps_src.life_time.secs_max ? ps_src.life_time.secs_min : GetRandom(ps_src.life_time.secs_min, ps_src.life_time.secs_max),
                // .life_secs_cur       //  calc on gpu
                // .life_time_alpha     //  calc on gpu
                .secs_to_start = ps_src.life_time.secs_to_start_max == 0.f ? 0.f : GetRandom(0.f, ps_src.life_time.secs_to_start_max),
                .pos_start = {},
                .pos_cur = {},     //  on start sets on cpu, changes only on gpu
                .dir_move_normalized = {1.f, 0.f, 0.f},     //  BUBBLE
                // .dir_move_normalized = pos == ZC_Vec3<float>() ? ZC_Vec3<float>() : ZC_Vec::Normalize(pos),      //  FLAME
                .move_speed_secs = ps_src.move.speed_min == ps_src.move.speed_max ? ps_src.move.speed_min
                    : G_ParticleSystem::GetRandom(ps_src.move.speed_min, ps_src.move.speed_max),
                // .uvs_id              //  calc on gpu
                // .uvs_cur_id          //  calc on gpu
            });
    }
    
    FillParticlesPosition(particles_temp);

    return particles_temp;
}

void G_ParticleSystem::FillParticlesPosition(std::vector<G_ParticleSystem::Particle>& rParticles)
{
    switch (ps_src.spawn_shape.shape)
    {
    case G_PS_Source::SpawnShape::S__Circle: FillShapeSphera(rParticles); break;
    case G_PS_Source::SpawnShape::S__Sphere: FillShapeCircle(rParticles); break;
    case G_PS_Source::SpawnShape::S__Cube: break;
    }
}

void G_ParticleSystem::FillShapeSphera(std::vector<G_ParticleSystem::Particle>& rParticles)
{
    float radius_min = ps_src.spawn_shape.val_1;
    float radius_max = ps_src.spawn_shape.val_2;
    bool random_radius = radius_min != radius_max;

    float rot_angle = ZC_angle_360f / std::floor(std::sqrt(ps_src.particles_count));   //  round to integer count of particles in mesh

    ul_zc rParticles_i = 0;
    for (float cur_angle_X = 0.f; cur_angle_X < ZC_angle_360f; cur_angle_X = cur_angle_X + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_X + rot_angle)
    {
        for (float cur_angle_Z = 0.f; cur_angle_Z < ZC_angle_360f; cur_angle_Z = cur_angle_Z + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_Z + rot_angle, ++rParticles_i)
        {       //  calculate position
            ZC_Mat4<float> model(1.f);
            if (cur_angle_X != 0.f) model.Rotate(cur_angle_X, { 1.f, 0.f, 0.f});
            if (cur_angle_Z != 0.f) model.Rotate(cur_angle_Z, { 0.f, 0.f, 1.f});
            ZC_Vec3<float> pos = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, random_radius ? float(GetRandom(radius_min, radius_max)) : radius_min, 0.f, 1.f));
            FillParticlePosition(rParticles[rParticles_i], pos);
        }
    }

    size_t rest_sphere_particles = rParticles.size() - rParticles_i;
    for (; rParticles_i < rest_sphere_particles; ++rParticles_i)  //  sphere mesh may not be edeal, so rest particles add with random rotation
    {
        ZC_Vec3<float> pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4(1.f).Rotate(ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i), {1.f, 1.f, 1.f})
            * ZC_Vec4<float>(0.f, random_radius ? float(GetRandom(radius_min, radius_max)) : radius_min, 0.f, 1.f));
        FillParticlePosition(rParticles[rParticles_i], pos);
    }
}

// ZC_Vec2<float> G_ParticleSystem::CalcRandPosXYInCircle()
// {
//     float dir_x = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
//     float dir_y = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
//     float dist = ps_src.spawn_shape.val_1 == ps_src.spawn_shape.val_2 ? ps_src.spawn_shape.val_1 : GetRandom(ps_src.spawn_shape.val_1, ps_src.spawn_shape.val_2);
//     ZC_Vec2<float> pos_xy = ZC_Vec::MoveByLength({}, { dir_x, dir_y }, dist);

//     enum Quater
//     {                       //  If +x to right, +y to up
//         x_neg__y_pos,       //  tl
//         x_pos__y_pos,       //  tr
//         x_pos__y_neg,       //  br
//         x_neg__y_neg        //  bl
//     } static last_quater = x_neg__y_pos;
//     switch (last_quater)  //  each new particle have pos at the next quater of th ecircle with clockwise
//     {
//     case x_neg__y_pos: pos_xy[0] *= -1.f; break;
//     case x_pos__y_pos: break;   //  default values are positive
//     case x_pos__y_neg: pos_xy[1] *= -1.f; break;
//     case x_neg__y_neg: pos_xy *= -1.f; break;
//     }
//     last_quater = Quater(last_quater + 1);
//     if (last_quater > x_neg__y_neg) last_quater = x_neg__y_pos;
//     return pos_xy;
// }

void G_ParticleSystem::FillShapeCircle(std::vector<G_ParticleSystem::Particle>& rParticles)
{
    for (Particle& p : rParticles)
    {
        float dir_x = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
        float dir_y = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
        float dist = ps_src.spawn_shape.val_1 == ps_src.spawn_shape.val_2 ? ps_src.spawn_shape.val_1 : GetRandom(ps_src.spawn_shape.val_1, ps_src.spawn_shape.val_2);
        ZC_Vec2<float> pos_xy = ZC_Vec::MoveByLength({}, { dir_x, dir_y }, dist);

        enum Quater
        {                       //  If +x to right, +y to up
            x_neg__y_pos,       //  tl
            x_pos__y_pos,       //  tr
            x_pos__y_neg,       //  br
            x_neg__y_neg        //  bl
        } static last_quater = x_neg__y_pos;
        switch (last_quater)  //  each new particle have pos at the next quater of th ecircle with clockwise
        {
        case x_neg__y_pos: pos_xy[0] *= -1.f; break;
        case x_pos__y_pos: break;   //  default values are positive
        case x_pos__y_neg: pos_xy[1] *= -1.f; break;
        case x_neg__y_neg: pos_xy *= -1.f; break;
        }
        last_quater = Quater(last_quater + 1);
        if (last_quater > x_neg__y_neg) last_quater = x_neg__y_pos;

        FillParticlePosition(p, { pos_xy[0], pos_xy[1], 0.f });
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

    ps.prev_frame_secs = time;
    ps.total_secs += time;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, prev_frame_secs), sizeof(ps.prev_frame_secs) + sizeof(ps.total_secs), &ps.prev_frame_secs);

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
    ps.prev_frame_secs = time;
    ps.total_secs += time;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticleSystem, prev_frame_secs), sizeof(ps.prev_frame_secs) + sizeof(ps.total_secs), &ps);
    // G_ParticleSystem::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds
#endif
}

//  sum (cpu + gpu) 1000 particles
//  0.000064 - 0.00011     update time to uniform (power off)
//  0.000147     update pos at cpu
//  0.000065 - 0.000135      ssbo, update time at cpu, all data particle data updates at vertex shader