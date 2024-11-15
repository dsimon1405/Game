#include "G_ParticlesDrawer.h"

#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>

G_ParticlesDrawer::~G_ParticlesDrawer()
{
    ec_updater.Disconnect();
}

void G_ParticlesDrawer::SetDrawState(bool need_draw)
{
    ZC_DrawerLevel dl_new = need_draw ? drawer_level : ZC_DL_None;
    assert(dl_new != ds_con.GetDrawingLevel(ZC_RL_Default));
    ds_con.SwitchToDrawLvl(ZC_RL_Default, dl_new);
    
    if (dl_new == ZC_DL_None)
    {
        ec_updater.Disconnect();
        DrawingStopped();
    }
    else if (!ec_updater.IsConnected())
        ec_updater.NewConnection(ZC__Updater::Connect({ &G_ParticlesDrawer::Callback_Updater, this }, G_UL__game_particles));   //  connect to update if it is not yet
}

void G_ParticlesDrawer::SetAlpha(float alpha)
{
    assert(alpha <= 1.f && alpha >= 0.f);
    unAlpha = alpha > 1.f ? 1.f : alpha < 0.f ? 0.f : alpha;
}

float G_ParticlesDrawer::GetAlpha() const noexcept
{
    return unAlpha;
}

bool G_ParticlesDrawer::IsDrawing()
{
    return ds_con.GetDrawingLevel(ZC_RL_Default) == drawer_level;
}

G_ParticlesDrawer::G_ParticlesDrawer(unsigned long long particles_count, ZC_DrawerLevel _drawer_level, int point_size)
    : drawer_set(CreateDrawerSet(particles_count)),
    ds_con(drawer_set.MakeZC_DSController()),
    drawer_level(_drawer_level)
{
        //  just reserve place, fill in AddPosition(), AddColor()
    particle_positions.reserve(particles_count);
    particle_colors.reserve(particles_count);

    ds_con.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
    ds_con.SetUniformsData(G_UN_unPointSize, &point_size);
}

void G_ParticlesDrawer::AddPositionAndColor(const ZC_Vec4<float>& pos, const ZC_Vec3<unsigned char>& color, ZC_Vec4<float>*& rpPos, ZC_Vec3<unsigned char>*& rpColor)
{
    assert(particle_positions.size() < particle_positions.capacity());
    rpPos = &(particle_positions.emplace_back(pos));
    rpColor = &(particle_colors.emplace_back(color));
    if (particle_positions.size() == particle_positions.capacity())   //  positoins and colors filled filled, update gpu
    {
        GLSubDataPositions();
        GLSubDataColors();
    }
}

ZC_Vec3<unsigned char> G_ParticlesDrawer::CalculateColor(const ZC_Vec3<unsigned char>& color_start, const ZC_Vec3<unsigned char>& color_end)
{
    return
        {
            color_end[0] >= color_start[0] ? uchar(color_start[0] + ZC_Random::GetRandomInt(0, color_end[0] - color_start[0]))
                : uchar(color_end[0] + ZC_Random::GetRandomInt(0, color_start[0] - color_end[0])),
            color_end[1] >= color_start[1] ? uchar(color_start[1] + ZC_Random::GetRandomInt(1, color_end[1] - color_start[1]))
                : uchar(color_end[1] + ZC_Random::GetRandomInt(0, color_start[1] - color_end[1])),
            color_end[2] >= color_start[2] ? uchar(color_start[2] + ZC_Random::GetRandomInt(0, color_end[2] - color_start[2]))
                : uchar(color_end[2] + ZC_Random::GetRandomInt(0, color_start[2] - color_end[2]))
        };
}

ZC_DrawerSet G_ParticlesDrawer::CreateDrawerSet(unsigned long long particles_count)
{
	ZC_Buffer vbo(GL_ARRAY_BUFFER);
	vbo.GLNamedBufferStorage((particles_count * sizeof(ZC_Vec4<float>)) + (particles_count * sizeof(ZC_Vec3<unsigned char>)), nullptr, GL_DYNAMIC_STORAGE_BIT);

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArraysInstanced>(GL_POINTS, 0, particles_count, 1);
	// ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, particles_count);

	typename ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(SHPN_Game_Particle);

	ZC_VAO vao;
	vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, particles_count);

	std::forward_list<ZC_Buffer> buffers;
	buffers.emplace_front(std::move(vbo));

    return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers));
}

// #define G_ParticlesDrawer_Callback_Updater
#ifdef G_ParticlesDrawer_Callback_Updater
#include <ZC/Tools/Time/ZC_Timer.h>
#include <iostream>
#endif
void G_ParticlesDrawer::Callback_Updater(float time)
{
#ifdef G_ParticlesDrawer_Callback_Updater
    static ZC_Timer timer_update(ZC_TR__seconds, 2., ZC_TRO__average, "update");
    static ZC_Timer timer_sub_data(ZC_TR__seconds, 2., ZC_TRO__average, "sub_data");
    timer_update.StartPoint();
    UpdateParticlesPositions(time);
    timer_update.EndPoint();
    timer_sub_data.StartPoint();
    GLSubDataPositions();
    timer_sub_data.EndPoint();
    if (timer_update.GetValuesCount() == 0ul) std::cout<<std::endl;
 #else
    if (UpdateParticlesPositions(time)) GLSubDataPositions();
#endif
        //  call heirs apdate functions, update gpu if need
    if (UpdateParticlesColors(time)) GLSubDataColors();
}

void G_ParticlesDrawer::GLSubDataPositions()
{
    drawer_set.buffers.front().GLNamedBufferSubData(0, particle_positions.size() * sizeof(ZC_Vec4<float>), particle_positions.data());  //  update positions on gpu (at gpu first goes positoins, then colors)
}

void G_ParticlesDrawer::GLSubDataColors()
{
    drawer_set.buffers.front().GLNamedBufferSubData(particle_positions.size() * sizeof(ZC_Vec4<float>),
        particle_colors.size() * sizeof(ZC_Vec3<unsigned char>), particle_colors.data());  //  update colors on gpu (at gpu first goes positoins, then colors)
}