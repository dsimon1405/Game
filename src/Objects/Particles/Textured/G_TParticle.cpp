#include "G_TParticle.h"

#include <ZC/File/ZC_File.h>
#include <System/G_RenderLevel.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>

#include <vector>
#include <string>


G_TParticle::G_TParticle(ul_zc count, float appear_circle_radius, float life_secs_min, float life_secs_max)
    : upSP(new SetupParticle()),
    particles_data{},
    particles(),
    pos_and_life_time{},
    ds(CreateDrawerSet(upSP->data.particles_count, upSP->data.circle_radius, upSP->data.life_secs_min, upSP->data.life_secs_max)),
    // ds(CreateDrawerSet(count, appear_circle_radius, life_secs_min, life_secs_max)),
    ds_con(ds.MakeZC_DSController(0))
{
    // ds_con.SetUniformsData(G_UN_unData, &unData);
    ds_con.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_ParticleFlame);

    ec_updater = ZC__Updater::Connect({ &G_TParticle::Callback_Updater, this }, G_UL__game_particles);
    
    pStatic = this;
    /*
    data[0].x - cur time in seconds
    data[0].y - tiles per second
    data[0].z - particle widht
    data[0].w - particle height

    data[1].x - appear/disappear seconds. other time particle have alpha 1.f
    data[1].y - pos x
    data[1].z - pos y
    data[1].y - pos z

    data[2].x - move to x
    data[2].y - move to y
    data[2].z - move to z
    data[2].w - move speed
    */
    unData[0][1] = upSP->data.tiles_per_second;
    unData[0][2] = upSP->data.width;
    unData[0][3] = upSP->data.height;
    
    unData[1][0] = upSP->data.appear_secs;
    unData[1][1] = upSP->data.pos_x;
    unData[1][2] = upSP->data.pos_y;
    unData[1][3] = upSP->data.pos_z;

    unData[2][0] = upSP->data.move_to_x;
    unData[2][1] = upSP->data.move_to_y;
    unData[2][2] = upSP->data.move_to_z;
    unData[2][3] = ZC_Random::GetRandomInt(upSP->data.move_speed_min_sec * f_100, upSP->data.move_speed_max_sec * f_100) / f_100;
    int q = 3;
}

G_TParticle::~G_TParticle()
{
    ec_updater.Disconnect();
}

float G_TParticle::CalcLifeTime(float secs_min, float secs_max)
{
    return f_zc(ZC_Random::GetRandomInt(secs_min * f_100, secs_max * f_100)) / f_100;
}

ZC_Vec2<float> G_TParticle::CalcRandPosXYInCircle(float circle_radius)
{
    float dir_x = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
    float dir_y = f_zc(ZC_Random::GetRandomInt(1, i_100)) / f_100;
    float dist = f_zc(ZC_Random::GetRandomInt(0, circle_radius * f_100)) / f_100;
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
    return pos_xy;
}

// #include <iostream>
ZC_DrawerSet G_TParticle::CreateDrawerSet(ul_zc count, float appear_circle_radius, float life_secs_min, float life_secs_max)
{
    count = 2;
    particles.reserve(count);
    // particles = std::vector<Particle>(count, Particle{});
    for (ul_zc i = 0; i < particles.capacity(); ++i)
    {
        particles.emplace_back(Particle
            {
                .secs_to_start = 0.f,
                .pos_start = i == 0 ? ZC_Vec3<float>(-2, 0, 0) : ZC_Vec3<float>(2, 0, 0),
                .pos_cur = i == 0 ? ZC_Vec3<float>(-2, 0, 0) : ZC_Vec3<float>(2, 0, 0),     //  on start sets on cpu, changes only on gpu
                .life_secs_total = 4.f,
                // .life_secs_cur       //  calc on gpu
                .dir_move_normalized = { 0, 0, 1 },
                .move_speed_secs = 0.5f,
                // .uvs_id              //  calc on gpu
            });
    }

    float uv_per_sec = 1.f;
    particles_data = ParticlesData
        {
            // .prev_frame_secs = ,     //  calc on gpu
            // .total_secs = ,          //  calc on gpu
            .particles_origin_pos = { 0.f, 0.f, 0.f },
            // .bl = ,                  //  calc on gpu
            // .br = ,                  //  calc on gpu
            // .tl = ,                  //  calc on gpu
            // .tr = ,                  //  calc on gpu
            .half_width = 1.f,
            .half_height = 1.f,
            .uv_shift_speed = 1.f / uv_per_sec,
            .appear_secs = 0.4f,
            .disappear_secs = 0.8f,
        };

    ZC_Buffer ssbo_particles(GL_SHADER_STORAGE_BUFFER, BIND_SSBO_PARTICLE);
    ssbo_particles.GLNamedBufferStorage(sizeof(ParticlesData) + (sizeof(Particle) * particles.size()), &particles_data, GL_DYNAMIC_STORAGE_BIT);
    ssbo_particles.GLNamedBufferSubData(sizeof(ParticlesData), sizeof(Particle) * particles.size(), particles.data());

    const int tex_width = 512.f;
    const int tex_height = 512.f;
    const int tex_columns_count = 4;
    const int tex_rows_count = 4;
    const int tile_width = tex_width / tex_columns_count;
    const int tile_heihgt = tex_height / tex_rows_count;
    uint uvs_count = tex_columns_count * tex_rows_count;
    std::vector<UV> uvs;
    uvs.reserve(uvs_count);
    for (int row_i = 0, tile_i = 0; row_i < tex_rows_count; ++row_i)
    {
        for (int column_i = 0; column_i < tex_columns_count; ++column_i)
        {       //  fill uv from top left corner of the texture to bottom right
            float left_x = column_i * tile_width;
            float top_y = tex_height - (row_i * tile_heihgt);  //  opengl read textures from bottom to top
            uvs.emplace_back(UV
                {
                    .left_x = left_x / tex_width,
                    .top_y = top_y / tex_height,
                    .right_x = (left_x + tile_width) / tex_width,
                    .bottom_y = (top_y - tile_heihgt) / tex_width,
                });
        }
    }
    ZC_Buffer ssbo_uvs(GL_SHADER_STORAGE_BUFFER, BIND_SSBO_TEX_DATA);
    ssbo_uvs.GLNamedBufferStorage(sizeof(uvs_count) + (sizeof(UV) * uvs.size()), &uvs_count, GL_DYNAMIC_STORAGE_BIT);
    ssbo_uvs.GLNamedBufferSubData(sizeof(uvs_count), sizeof(UV) * uvs.size(), uvs.data());

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, count);

    ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

	ZC_VAO vao;

	std::forward_list<ZC_Buffer> buffers;
    buffers.emplace_front(std::move(ssbo_uvs));
	buffers.emplace_front(std::move(ssbo_particles));

	std::forward_list<ZC_TexturesSet> tex_sets;
    ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
    texCreator.Add(ZC_Texture::LoadTexture2D(ZC_FSPath(ZC_assetsDirPath).append("Game/textures/flame.png").string().c_str(), 0, GL_REPEAT, GL_REPEAT));
    tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = texCreator.GetVector() });

    return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets));
}

#define G_TParticle_Callback_Updater
#ifdef G_TParticle_Callback_Updater
#include <ZC/Tools/Time/ZC_Timer.h>
#include <iostream>
#endif
void G_TParticle::Callback_Updater(float time)
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

    particles_data.prev_frame_secs = time;
    particles_data.total_secs += time;
    ds.buffers.front().GLNamedBufferSubData(offsetof(ParticlesData, prev_frame_secs), sizeof(particles_data.prev_frame_secs) + sizeof(particles_data.total_secs), &particles_data);

    // static ZC_Clock cl;
#ifdef G_TParticle_Callback_Updater
    static ZC_Timer timer(ZC_TR__repeats, 5000., ZC_TRO__average, "updt flame");

    timer.StartPoint();

        //  update time to uniform
    // G_TParticle::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds

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
    G_TParticle::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds
#endif
}

//  sum
//  0.000064 / 0.00011     update time to uniform (power off)
//  0.000147     update pos at cpu