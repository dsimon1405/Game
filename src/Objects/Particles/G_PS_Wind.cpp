#include "G_PS_Wind.h"

#include <System/G_RenderLevel.h>
#include <ZC/Tools/Math/ZC_Vec.h>
#include <ZC/Tools/ZC_Random.h>

G_PS_Wind::G_PS_Wind(unsigned long particles_count, const ZC_Vec3<float>& _start_pos, float start_width, float start_height,
        const ZC_Vec3<float>& _wind_dir, float _length, float _speed, const ZC_Vec3<unsigned char>& color_start, const ZC_Vec3<unsigned char>& color_end)
    : G_ParticlesDrawer(particles_count, G_DL_AlphaBlending_ParticlesWind, 3),
    wind_pos(_start_pos),
    wind_dir(_wind_dir == ZC_Vec3<float>() ? ZC_Vec3<float>() : ZC_Vec::Normalize(_wind_dir)),
    length(_length),
    speed(_speed)
{
    particles.reserve(particles_count);     //  reserve place for particles
    for (size_t i = 0; i < particles_count; i++)
    {
            //  calculate random start pos for particle and set with object pos (pointer on wind_pos)
            //  take random point in radius distance on +Z axis, and rotate it around Y axis (start front allways +Y), then around axis Z on random angle too, will have some random sphere of start points
        // float dist_from_circle_start_pos = float(ZC_Random::GetRandomInt(0, start_sphere_radius * 100)) / 100.f;
        // float rot_angle_Y = ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i);
        // float rot_angle_Z = ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i);
        // ZC_Vec3<float> random_particle_start_pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(rot_angle_Z, { 0.f, 0.f, 1.f }).
        //     Rotate(rot_angle_Y, { 0.f, 1.f, 0.f }) * ZC_Vec4<float>(0.f, 0.f, dist_from_circle_start_pos, 1.f));

        float half_width = start_width / 2.f * 100.f;   //  100 then will divided
        float rand_x = float(ZC_Random::GetRandomInt(- half_width, half_width)) / 100.f;
        float rand_y = float(ZC_Random::GetRandomInt(- half_width, half_width)) / 100.f;
        float rand_z = float(ZC_Random::GetRandomInt(0, start_height * 100.f)) / 100.f;

            //  life time and speed (take speed and tile time in range [80 - 100]%)
        float particle_speed = float(ZC_Random::GetRandomInt(30, 100)) / 100.f;
        float life_time = _length * (1.f + (1.f - particle_speed));

        ZC_Vec4<float>* pPos_alpha = nullptr;
        ZC_Vec3<unsigned char>* pColor = nullptr;
        this->AddPositionAndColor({ rand_x, rand_y, rand_z, 1.f }, this->CalculateColor(color_start, color_end), pPos_alpha, pColor);
        particles.emplace_back(pPos_alpha, pColor, &wind_pos, &wind_dir, particle_speed, life_time);      //  add particle
    }
}

void G_PS_Wind::SetPosition(const ZC_Vec3<float>& _start_pos)
{
    wind_pos = _start_pos;
}

void G_PS_Wind::SetWindDiraction(const ZC_Vec3<float>& _wind_dir)
{
    wind_dir = ZC_Vec::Normalize(_wind_dir);
}

void G_PS_Wind::SetSpeed(float _speed)
{
    assert(_speed >= 0.f);
    speed = _speed < 0.f ? 0.f : _speed;
}

bool G_PS_Wind::UpdateParticlesPositions(float time)
{
    for (G_ParticleMoveForward<ZC_Vec3<float>*>& p : particles) p.Update(time, speed);
    return true;
}

void G_PS_Wind::DrawingStopped()
{
    for (G_ParticleMoveForward<ZC_Vec3<float>*>& p : particles) p.SetDefaultState();
}