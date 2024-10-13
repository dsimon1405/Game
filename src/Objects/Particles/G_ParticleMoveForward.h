#pragma once

#include <ZC/Tools/Math/ZC_Vec4.h>

#include <concepts>

template <typename T>
concept G_cParticleDir = std::same_as<T, ZC_Vec3<float>> || std::same_as<T, ZC_Vec3<float>*>;

template <G_cParticleDir TDir>
struct G_ParticleMoveForward
{
    /*
    - pPos_cur__last_alpha - pointer from G_ParticlesDrawer::particle_positions. Value in pointer will sets as start position off the object space. On restart particle live =>  pPos_cur__last_alpha = start_pos + *pObject_pos;
    - _pColor - pointer from G_ParticlesDrawer::particle_colors.
    - _pObject_pos - pointer on object position. Example G_PS_Wind::start_pos.
    - dir - coud be value or pointer. Pointer sets if all G_particle have same move direction as in G_PS_Wind. Otherwise sets own value of direction for each particle.
    - speed - particle speed.
    - life_time - particle life time.
    */
    G_ParticleMoveForward(ZC_Vec4<float>* _pPos_cur__last_alpha, ZC_Vec3<unsigned char>* _pColor, ZC_Vec3<float>* _pObject_pos, TDir dir, float speed, float life_time);

    //     //  ctr for G_PMS__forward
    // void SetStartData_forwardMove();

        //  call G_Particles heir in ctr
    // void SetPoinerToObjectAndStartPos(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos);
    // void SetStartPosition(const ZC_Vec3<float>& pos);
       //  - dir - coud be value or pointer. Pointer sets if all G_particle have same move direction as in G_PS_Wind. Otherwise sets own value of direction for each particle.
    void SetMoveDirection(TDir dir);
    // void SetSpeedAndLifeTime(float _speed, float _life_tima);
    void SetDefaultState();
    void Update(float time, float total_speed);

private:
    float cur_time = 0.f;
        
        //  On restart particle live =>  pPos_cur__last_alpha = start_pos + *pObject_pos;
    ZC_Vec4<float>* pPos_cur__last_alpha;           //  source value in G_Particles::particle_positions. position of object in current life time
    ZC_Vec3<float>* pObject_pos;        //  pointer on object position to recalculate pPos_cur__last_alpha (sets once by owner)
    ZC_Vec3<float> start_pos;     //  start position calculated in constructor of owner (from origin {0,0,0})
    
    TDir dir_new;   //  coud be value or pointer. Pointer sets if all G_particle have same move direction as in G_PS_Wind. Otherwise sets own value of direction for each particle.
    ZC_Vec3<float> dir_cur;

    float speed_new;    //  determine G_PS_Wind or other G_PS
    float speed_cur;    //  determine G_PS_Wind or other G_PS
    
    float life_time_new;    //  determine G_PS_Wind or other G_PS
    float life_time_cur;    //  determine G_PS_Wind or other G_PS

    ZC_Vec3<unsigned char>* pColor;     //  source in G_Particles::particle_colors. Determine G_PS_Wind or other G_PS

    void UpdateData();
};

template <G_cParticleDir TDir>
G_ParticleMoveForward<TDir>::G_ParticleMoveForward(ZC_Vec4<float>* _pPos_cur__last_alpha, ZC_Vec3<unsigned char>* _pColor, ZC_Vec3<float>* _pObject_pos, TDir dir, float speed, float life_time)
    : pPos_cur__last_alpha(_pPos_cur__last_alpha),
    pObject_pos(_pObject_pos),
    start_pos((*pPos_cur__last_alpha)[0], (*pPos_cur__last_alpha)[1], (*pPos_cur__last_alpha)[2]),
    dir_new(dir),
    speed_new(speed),
    life_time_new(life_time),
    pColor(_pColor)
{}

// template <G_cParticleDir TDir>
// void G_ParticleMoveForward<TDir>::SetPoinerToObjectAndStartPos(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos)
// {
//     pObject_pos = _pObject_pos;
//     start_pos = _start_pos;
// }

// template <G_cParticleDir TDir>
// void G_ParticleMoveForward<TDir>::SetStartPosition(const ZC_Vec3<float>& pos)
// {
//     start_pos_new = pos;
//     if (cur_time == 0.f) *pPos_cur__last_alpha = start_pos_new;
// }

template <G_cParticleDir TDir>
void G_ParticleMoveForward<TDir>::SetMoveDirection(TDir dir)
{
    dir_new = dir;
    if (cur_time == 0.f)
    {
        if constexpr (std::same_as<TDir, ZC_Vec3<float>*>)
        {
            dir_cur = *dir_new;
        }
        else dir_cur = dir_new;
    }
}

// template <G_cParticleDir TDir>
// void G_ParticleMoveForward<TDir>::SetSpeedAndLifeTime(float _speed, float _life_tima)
// {
//     speed_new = _speed;
//     life_time_new = _life_tima;
//     if (cur_time == 0.f)
//     {
//         speed_cur = speed_new;
//         life_time_cur = life_time_new;
//     }
// }

template <G_cParticleDir TDir>
void G_ParticleMoveForward<TDir>::SetDefaultState()
{
    cur_time = 0.f;
}

template <G_cParticleDir TDir>
void G_ParticleMoveForward<TDir>::Update(float time, float total_speed)
{
    assert(total_speed >= 0);
    if (cur_time == 0.f) UpdateData();
    cur_time += time;
        //  if total speed > 1.f life time must became smaller, if greater larger to save move distance, caurse partical will move faster or slower
    if (cur_time > life_time_cur / (total_speed == 0 ? 1.f : total_speed)) SetDefaultState();
    else
    {
        static const float seconds_apear_disapear = 1.f;    //  make smooth apperaing of the particle
        // static const float seconds_apear_disapear = 0.5f;    //  make smooth apperaing of the particle

        float alpha = 0.f;
        if (cur_time < seconds_apear_disapear) alpha = cur_time / seconds_apear_disapear;     //  particle apear
        else if (cur_time > life_time_cur - seconds_apear_disapear) alpha = 1.f - (cur_time - (life_time_cur - seconds_apear_disapear)) / seconds_apear_disapear; //  particle disappear
        else alpha = 1.f;   //  not transparent

        ZC_Vec3<float> pos((*pPos_cur__last_alpha)[0], (*pPos_cur__last_alpha)[1], (*pPos_cur__last_alpha)[2]); //  last is alpha don't touch alpha
        pos += dir_cur * speed_cur * (total_speed == 0 ? 1.f : total_speed) * time;
        *pPos_cur__last_alpha = ZC_Vec4<float>(pos, alpha);
    }
}

template <G_cParticleDir TDir>
void G_ParticleMoveForward<TDir>::UpdateData()
{
    *pPos_cur__last_alpha = ZC_Vec4<float>(*pObject_pos + start_pos, 0.f);   //  last is alpa, make invisible on start
    if constexpr (std::same_as<TDir, ZC_Vec3<float>*>)
    {
        dir_cur = *dir_new;
    }
    else dir_cur = dir_new;
    speed_cur = speed_new;
    life_time_cur = life_time_new;
}