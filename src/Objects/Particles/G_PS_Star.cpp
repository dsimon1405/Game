#include "G_PS_Star.h"

#include <System/G_RenderLevel.h>
#include <ZC/Tools/Math/ZC_Vec.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/Tools/Math/ZC_Math.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

G_PS_Star::G_PS_Star(unsigned long long particles_count, const ZC_Vec3<float>& _star_pos, float _radius, float _speed, const ZC_Vec3<float>& rotate_axises,
        const ZC_Vec3<unsigned char>& color_start, const ZC_Vec3<unsigned char>& color_end)
    : G_ParticlesDrawer(particles_count, G_DL_AlphaBlending_ParticleStar, 3),
    star_pos(_star_pos),
    speed(_speed)
{
    auto lamb_AddParticleToCircle = [this, &color_start, &color_end, &rotate_axises](const ZC_Vec3<float>& pos)
    {
        float particle_speed = float(ZC_Random::GetRandomInt(100, 300)) / 100.f;

        ZC_Vec4<float>* pPos_alpha = nullptr;
        ZC_Vec3<unsigned char>* pColor = nullptr;
        this->AddPositionAndColor({ pos, 1.f }, this->CalculateColor(color_start, color_end), pPos_alpha, pColor);
        particles_sphere.emplace_back(pPos_alpha, pColor, &star_pos, rotate_axises, particle_speed);      //  add particle
    };

    const float radius_min = _radius * 1 * 100.f;
    const float radius_max = _radius * 1.4 * 100.f;
    auto lamb_GetStartPos = [radius_min, radius_max, _radius]()
    {
        return ZC_Vec4<float>(0.f, float(ZC_Random::GetRandomInt(radius_min, radius_max)) / 100.f, 0.f, 1.f);
    };

    float rot_angle = ZC_angle_360f / std::floor(std::sqrt(particles_count));   //  round to ineger count of particles in mesh
    particles_sphere.reserve(particles_count);
    for (float cur_angle_X = 0.f; cur_angle_X < ZC_angle_360f; cur_angle_X = cur_angle_X + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_X + rot_angle)
    {
        for (float cur_angle_Z = 0.f; cur_angle_Z < ZC_angle_360f; cur_angle_Z += cur_angle_Z + rot_angle > 359.9f ? ZC_angle_360f : cur_angle_Z + rot_angle)
        {       //  calculate position
            ZC_Mat4<float> model(1.f);
            if (cur_angle_X != 0.f) model.Rotate(cur_angle_X, { 1.f, 0.f, 0.f});
            if (cur_angle_Z != 0.f) model.Rotate(cur_angle_Z, { 0.f, 0.f, 1.f});
            // lamb_AddParticleToCircle(ZC_Vec::Vec4_to_Vec3(model * start_pos));
            lamb_AddParticleToCircle(ZC_Vec::Vec4_to_Vec3(model * lamb_GetStartPos()));
        }
    }
    size_t rest_sphere_particles = particles_sphere.capacity() - particles_sphere.size();
    for (size_t i = 0; i < rest_sphere_particles; i++)  //  sphere mesh may not be edeal, so rest particles add with random rotation
        // lamb_AddParticleToCircle(ZC_Vec::Vec4_to_Vec3(ZC_Mat4(1.f).Rotate(ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i), {1.f, 1.f, 1.f}) * start_pos));
        lamb_AddParticleToCircle(ZC_Vec::Vec4_to_Vec3(ZC_Mat4(1.f).Rotate(ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i), {1.f, 1.f, 1.f}) * lamb_GetStartPos()));
    
}

void G_PS_Star::SetPosition(const ZC_Vec3<float>& pos)
{
    star_pos = pos;
}

ZC_Vec3<float> G_PS_Star::GetPosition() const noexcept
{
    return star_pos;
}

void G_PS_Star::SetSpeed(float _speed)
{
    speed = _speed;
}

float G_PS_Star::GetSpeed() const noexcept
{
    return speed;
}

bool G_PS_Star::UpdateParticlesPositions(float time)
{
    for (G_ParticleMoveCircle& p : particles_sphere) p.Update(time, speed);
    return true;
}