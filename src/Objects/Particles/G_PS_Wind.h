#pragma once

#include "G_ParticlesDrawer.h"
#include "G_ParticleMoveForward.h"

class G_PS_Wind : public G_ParticlesDrawer
{
public:
    /*
    Origin(start pos) for wind sets in G_Particles!

    - particles_count - particles count.
    - _start_pos - start position of the wind. Particles will apear appear by Z.
    - start_width - width of the quad in wich particles will appear, with _start_pos like origin.
    - start_height - height of the quad for appear particles.
    - _wind_dir - wind direction (may be not normalized).
    - _length - length of the wind from _start_sphere_radius in _wind_dir direction.
    - speed - speed coef, default (1 meter(float) / 1 second).
    - _color_start, _color_end - range for colors (each particle will have random value from that range).
    */
    G_PS_Wind(unsigned long long particles_count, const ZC_Vec3<float>& _start_pos, float start_width, float start_height, const ZC_Vec3<float>& _wind_dir, float _length,
        float _speed = 1.f, const ZC_Vec3<unsigned char>& _color_start = { 175, 238, 238 }, const ZC_Vec3<unsigned char>& _color_end = { 0, 206, 209 });

        // - _start_sphere_radius - radius in wich start to apear particles (only half of start sphere are in _length).
    void SetPosition(const ZC_Vec3<float>& _start_pos);
        // - _wind_dir - wind direction (may be not normalized).
    void SetWindDiraction(const ZC_Vec3<float>& _wind_dir);
        // - speed - speed coef, default (1 meter(float) / 1 second).
    void SetSpeed(float speed);

private:
    std::vector<G_ParticleMoveForward<ZC_Vec3<float>*>> particles;
    ZC_Vec3<float> wind_pos;   //  position of quad in wich apears particles on start, calculates in OriginUpdated()
    ZC_Vec3<float> wind_dir;
    float length;
    float speed;

    bool UpdateParticlesPositions(float time) override;
    void DrawingStopped() override;
};