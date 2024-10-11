#pragma once

#include "G_ParticleSet.h"
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>

template <G_cParticleDir TDir>
class G_ParticleDrawer
{
public:
    G_ParticleDrawer(std::vector<G_ParticleSet<TDir>>&& _particles_sets, ZC_DrawerLevel _drawer_level, float _speed);
    virtual ~G_ParticleDrawer() = default;
    
        //  start/stop drawing
    void SetDrawState(bool need_draw);
    void SetAlpha(float alpha);
    float GetAlpha() const noexcept;
        // - speed - speed coef, default (1 meter(float) / 1 second).
    void SetSpeed(float speed);
    bool IsDrawing();

protected:
    std::vector<G_ParticleSet<TDir>> particle_sets;

    void GLSubDataColors();

private:
    std::vector<ZC_Vec4<float>> particle_positions;     //  store all positions, last is alpha channel for gpu update with vertices, colors don't updates
    std::vector<ZC_Vec3<unsigned char>> particle_colors;    //  store all color
    
    ZC_DrawerSet drawer_set;
    ZC_DSController ds_con;
    ZC_DrawerLevel drawer_level;

    float speed;
    float unAlpha = 1.f;

    ZC_EC ec_updater;

    unsigned long GetParticlesCapacity();
    ZC_DrawerSet CreateDrawerSet();

    void Callback_Updater(float time);

    void GLSubDataPositions();
};



template <G_cParticleDir TDir>
G_ParticleDrawer<TDir>::G_ParticleDrawer(std::vector<G_ParticleSet<TDir>>&& _particles_sets, ZC_DrawerLevel _drawer_level, float _speed)
    : particle_sets(std::move(_particles_sets)),
    particle_positions(GetParticlesCapacity()),
    particle_colors(particle_positions.size()),
    drawer_set(CreateDrawerSet()),
    ds_con(drawer_set.MakeZC_DSController()),
    drawer_level(_drawer_level),
    speed(_speed)
{
        //  fill particle_sets with pointers
    unsigned long container_index = 0;    //  index from particle_positions and particle_colors
    for (G_ParticleSet<TDir>& ps : particle_sets)
        for (unsigned long particle_index = 0; particle_index < ps.particles.capacity(); ++particle_index, ++container_index)     //  set pointers on vertices and colors
            ps.particles.emplace_back(&(particle_positions[container_index]), &(particle_colors[container_index]));

    ds_con.SetUniformsData(ZC_UN_unAlpha, &unAlpha);
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::SetDrawState(bool need_draw)
{
    ZC_DrawerLevel dl_new = need_draw ? drawer_level : ZC_DL_None;
    assert(dl_new != ds_con.GetDrawingLevel(ZC_RL_Default));
    ds_con.SwitchToDrawLvl(ZC_RL_Default, dl_new);
    
    if (dl_new == ZC_DL_None)
    {
        ec_updater.Disconnect();
        for (G_ParticleSet<TDir>& ps : particle_sets)
            for (G_Particle<TDir>& p : ps.particles) p.SetDefaultState();
    }
    else if (!ec_updater.IsConnected())
        ec_updater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_ParticleDrawer<TDir>::Callback_Updater, this }, G_UL__game_particles));   //  connect to update if it is not yet
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::SetAlpha(float alpha)
{
    assert(alpha <= 1.f && alpha >= 0.f);
    unAlpha = alpha > 1.f ? 1.f : alpha < 0.f ? 0.f : alpha;
}

template <G_cParticleDir TDir>
float G_ParticleDrawer<TDir>::GetAlpha() const noexcept
{
    return unAlpha;
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::SetSpeed(float _speed)
{
    assert(_speed >= 0.f);
    speed = _speed < 0.f ? 0.f : _speed;
}

template <G_cParticleDir TDir>
bool G_ParticleDrawer<TDir>::IsDrawing()
{
    return ds_con.GetDrawingLevel(ZC_RL_Default) == drawer_level;
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::GLSubDataColors()
{
    drawer_set.buffers.front().GLNamedBufferSubData(particle_positions.size() * sizeof(ZC_Vec4<float>),
        particle_colors.size() * sizeof(ZC_Vec3<unsigned char>), particle_colors.data());  //  update colors on gpu (at gpu first goes positoins, then colors)
}

template <G_cParticleDir TDir>
unsigned long G_ParticleDrawer<TDir>::GetParticlesCapacity()
{
    unsigned long particles_count = 0;
    for (G_ParticleSet<TDir>& ps : particle_sets) particles_count += ps.particles.capacity();
    return particles_count;
}

template <G_cParticleDir TDir>
ZC_DrawerSet G_ParticleDrawer<TDir>::CreateDrawerSet()
{
	ZC_Buffer vbo(GL_ARRAY_BUFFER);
	vbo.GLNamedBufferStorage((particle_positions.size() * sizeof(ZC_Vec4<float>)) + (particle_colors.size() * sizeof(ZC_Vec3<unsigned char>)),
        nullptr, GL_DYNAMIC_STORAGE_BIT);

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, particle_positions.size());

	typename ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(SHPN_Game_Particle);

	ZC_VAO vao;
	vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, particle_positions.size());   //  vertices count in vbo (in one quad 4, in one triangle 3)

	std::forward_list<ZC_Buffer> buffers;
	buffers.emplace_front(std::move(vbo));

    return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers));
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::Callback_Updater(float time)
{
    for (G_ParticleSet<TDir>& ps : particle_sets)
    {
        float total_speed = speed * ps.speed;
        for (G_Particle<TDir>& p : ps.particles)
            p.Update(time, total_speed);  //  update ps positions
    }
    GLSubDataPositions();
}

template <G_cParticleDir TDir>
void G_ParticleDrawer<TDir>::GLSubDataPositions()
{
    drawer_set.buffers.front().GLNamedBufferSubData(0, particle_positions.size() * sizeof(ZC_Vec4<float>), particle_positions.data());  //  update positions on gpu (at gpu first goes positoins, then colors)
}