#pragma once

#include "G_ParticlesDrawer.h"
#include "G_ParticleMoveCircle.h"

class G_PS_Star : public G_ParticlesDrawer
{
public:
    /*
    Origin(start pos) for wind sets in G_Particles!

    - particles_count - particles_sphere count.
    - _star_pos - start position of the wind. Particles will apear appear by Z.
    - _radius - radius of start.
    - speed - speed coef, default (1 meter(float) / 1 second).
    - rotate_axis - axises for rotation of particles.
    - _color_start, _color_end - range for colors (each particle will have random value from that range).
    */
    G_PS_Star(unsigned long long particles_count, const ZC_Vec3<float>& _star_pos, float _radius, float _speed = 1.f, const ZC_Vec3<float>& rotate_axises = { 1.f, 0.f, 1.f },
        const ZC_Vec3<unsigned char>& _color_start = { 150, 150, 100 }, const ZC_Vec3<unsigned char>& _color_end = { 255, 255, 255 });
        // const ZC_Vec3<unsigned char>& _color_start = { 255, 100, 0 }, const ZC_Vec3<unsigned char>& _color_end = { 255, 255, 0 });

        // - _start_sphere_radius - radius in wich start to apear particles_sphere (only half of start sphere are in _length).
    void SetPosition(const ZC_Vec3<float>& pos);
    ZC_Vec3<float> GetPosition() const noexcept;
        // - speed - speed coef, default (1 meter(float) / 1 second).
    void SetSpeed(float _speed);
    float GetSpeed() const noexcept;

private:
    // std::vector<G_ParticleMoveForward<ZC_Vec3<float>*>> particles_sphere;
    std::vector<G_ParticleMoveCircle> particles_sphere;
    ZC_Vec3<float> star_pos;   //  position of quad in wich apears particles_sphere on start, calculates in OriginUpdated()
    float speed;

    bool UpdateParticlesPositions(float time) override;
};