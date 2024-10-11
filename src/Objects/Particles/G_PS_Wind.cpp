#include "G_PS_Wind.h"

// #include <ZC/Tools/ZC_Random.h>
// #include <ZC/Tools/Math/ZC_Mat4.h>
// #include <System/G_Func.h>
// #include <ZC/Tools/Math/ZC_Math.h>
#include <System/G_RenderLevel.h>
#include <ZC/Tools/Math/ZC_Vec.h>

G_PS_Wind::G_PS_Wind(unsigned long particles_count, const ZC_Vec3<float>& _start_pos, float start_width, float start_height, const ZC_Vec3<float>& _wind_dir, float _length,
        float _speed, const ZC_Vec3<unsigned char>& color_start, const ZC_Vec3<unsigned char>& color_end)
    : G_ParticleDrawer<ZC_Vec3<float>*>(CreateParticlesVector(particles_count, _length), G_DL_AlphaBlending_ParticlesWind, _speed),
    start_pos(_start_pos),
    wind_dir(_wind_dir == ZC_Vec3<float>() ? ZC_Vec3<float>() : ZC_Vec::Normalize(_wind_dir)),
    length(_length)
{
    for (G_ParticleSet<ZC_Vec3<float>*>& ps : this->particle_sets)
    {
        for (G_Particle<ZC_Vec3<float>*>& p : ps.particles)
        {
                //  calculate random start pos for particle and set with object pos (pointer on start_pos)
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
            float speed = float(ZC_Random::GetRandomInt(30, 100)) / 100.f;
            float life_time = _length * (1.f + (1.f - speed));
            // float life_time = float(ZC_Random::GetRandomInt(float(_length) * 0.7f * 100.f, _length * 100)) / 100.f;   //  default is move speed 1 float / 1 second, so length can be taken as seconds. Need to live minimum falf length
            // float life_time = _length;
            float time_to_start = float(ZC_Random::GetRandomInt(0, float(_length) * 0.8f * 100.f)) / 100.f;      //  start time take such that some particles coude allready reach the midle while some still appear

                //  fill color with random calue from start to end
            ZC_Vec3<unsigned char> color;
            color[0] = color_end[0] >= color_start[0] ? color_start[0] + ZC_Random::GetRandomInt(0, color_end[0] - color_start[0])
                : color_end[0] + ZC_Random::GetRandomInt(0, color_start[0] - color_end[0]);
            color[1] = color_end[1] >= color_start[1] ? color_start[1] + ZC_Random::GetRandomInt(1, color_end[1] - color_start[1])
                : color_end[1] + ZC_Random::GetRandomInt(0, color_start[1] - color_end[1]);
            color[2] = color_end[2] >= color_start[2] ? color_start[2] + ZC_Random::GetRandomInt(0, color_end[2] - color_start[2])
                : color_end[2] + ZC_Random::GetRandomInt(0, color_start[2] - color_end[2]);

            p.SetStartData_forwardMove(&start_pos, { rand_x, rand_y, rand_z }, &wind_dir, speed, life_time, time_to_start, color);
        }
    }
    
    this->GLSubDataColors();
}

void G_PS_Wind::SetStartPosition(const ZC_Vec3<float>& _start_pos)
{
    start_pos = _start_pos;
}

void G_PS_Wind::SetWindDiraction(const ZC_Vec3<float>& _wind_dir)
{
    wind_dir = ZC_Vec::Normalize(_wind_dir);
}

std::vector<G_ParticleSet<ZC_Vec3<float>*>> G_PS_Wind::CreateParticlesVector(unsigned long particles_count, float _length)
{
    std::vector<G_Particle<ZC_Vec3<float>*>> p;
    p.reserve(particles_count);
    std::vector<G_ParticleSet<ZC_Vec3<float>*>> ps;
    ps.reserve(1);
    ps.emplace_back(std::move(p), 1.f);
    return ps;
}