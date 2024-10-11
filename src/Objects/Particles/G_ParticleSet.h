#pragma once

#include "G_Particle.h"
#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>
#include <ZC/Tools/ZC_Random.h>

template <G_cParticleDir TDir>
struct G_ParticleSet
{
public:
    std::vector<G_Particle<TDir>> particles;
    float speed;
        
    G_ParticleSet(std::vector<G_Particle<TDir>>&& particles, float _speed);
    G_ParticleSet(G_ParticleSet&& ps);

    // unsigned long GetParticlesCapacity() const noexcept;
    // void SetPointers_Pos_Count(ZC_Vec3<float>* pPos, ZC_Vec3<unsigned char>* pColor);

    void Updater(float time, float total_speed);
};


template <G_cParticleDir TDir>
G_ParticleSet<TDir>::G_ParticleSet(std::vector<G_Particle<TDir>>&& _particles, float _speed)
    : particles(std::move(_particles)),
    speed(_speed)
{}

template <G_cParticleDir TDir>
G_ParticleSet<TDir>::G_ParticleSet(G_ParticleSet&& ps)
    : particles(std::move(ps.particles)),
    speed(ps.speed)
{}

template <G_cParticleDir TDir>
void G_ParticleSet<TDir>::Updater(float time, float total_speed)
{
    for (G_Particle<TDir>& p : particles) p.Update(time, total_speed * speed);
}