#pragma once

#include <ZC/Tools/Math/ZC_Vec4.h>

#include <concepts>

enum G_ParticleMoveStyle
{
    G_PMS__forward,
    G_PMS__circle
};

template <typename T>
concept G_cParticleDir = std::same_as<T, ZC_Vec3<float>> || std::same_as<T, ZC_Vec3<float>*>;

template <G_cParticleDir TDir>
struct G_Particle
{
    G_Particle(ZC_Vec4<float>* _pPos_cur, ZC_Vec3<unsigned char>* _pColor);

        //  ctr for G_PMS__forward
    void SetStartData_forwardMove(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos, TDir dir, float speed, float life_time, float _time_to_start,
        const ZC_Vec3<unsigned char>& color);

        //  call G_Particles heir in ctr
    // void SetPoinerToObjectAndStartPos(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos);
    // void SetStartPosition(const ZC_Vec3<float>& pos);
       //  - dir - coud be value or pointer. Pointer sets if all G_particle have same move direction as in G_PS_Wind. Otherwise sets own value of direction for each particle.
    void SetMoveDirection(TDir dir);
    // void SetSpeedAndLifeTime(float _speed, float _life_tima);
    void SetDefaultState();
    void UpdateData();
    void Update(float time, float total_speed);

private:
    float cur_time = 0.f;
    bool may_draw = false;
        
    G_ParticleMoveStyle pms;

        //  On restart particle live =>  pPos_cur__last_alpha = start_pos + *pObject_pos;
    ZC_Vec3<float>* pObject_pos = nullptr;        //  pointer on object position to recalculate pPos_cur__last_alpha (sets once by owner)
    ZC_Vec3<float> start_pos;     //  start position calculated in constructor of owner (from origin {0,0,0})
    ZC_Vec4<float>* pPos_cur__last_alpha = nullptr;           //  source value in G_Particles::particle_positions. position of object in current life time
    
    TDir dir_new;   //  coud be value or pointer. Pointer sets if all G_particle have same move direction as in G_PS_Wind. Otherwise sets own value of direction for each particle.
    ZC_Vec3<float> dir_cur;

    float speed_new;    //  determine G_PS_Wind or other G_PS
    float speed_cur;    //  determine G_PS_Wind or other G_PS
    
    float life_time_new;    //  determine G_PS_Wind or other G_PS
    float life_time_cur;    //  determine G_PS_Wind or other G_PS

    float time_to_start;    //  time to start apearing in new life time

    ZC_Vec3<unsigned char>* pColor;     //  source in G_Particles::particle_colors. Determine G_PS_Wind or other G_PS
};


template <G_cParticleDir TDir>
void G_Particle<TDir>::SetStartData_forwardMove(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos, TDir dir, float speed, float life_time, float _time_to_start,
    const ZC_Vec3<unsigned char>& color)
{
    pms = G_PMS__forward;
    pObject_pos = _pObject_pos;
    start_pos = _start_pos;
    dir_new = dir;
    speed_new = speed;
    life_time_new = life_time;
    *pColor = color;
    time_to_start = _time_to_start;
    SetDefaultState();
}

template <G_cParticleDir TDir>
G_Particle<TDir>::G_Particle(ZC_Vec4<float>* _pPos_cur, ZC_Vec3<unsigned char>* _pColor)
    : pPos_cur__last_alpha(_pPos_cur),
    pColor(_pColor)
{}

// template <G_cParticleDir TDir>
// void G_Particle<TDir>::SetPoinerToObjectAndStartPos(ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _start_pos)
// {
//     pObject_pos = _pObject_pos;
//     start_pos = _start_pos;
// }

// template <G_cParticleDir TDir>
// void G_Particle<TDir>::SetStartPosition(const ZC_Vec3<float>& pos)
// {
//     start_pos_new = pos;
//     if (cur_time == 0.f) *pPos_cur__last_alpha = start_pos_new;
// }

template <G_cParticleDir TDir>
void G_Particle<TDir>::SetMoveDirection(TDir dir)
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
// void G_Particle<TDir>::SetSpeedAndLifeTime(float _speed, float _life_tima)
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
void G_Particle<TDir>::SetDefaultState()
{
    cur_time = 0.f;
}

template <G_cParticleDir TDir>
void G_Particle<TDir>::UpdateData()
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

template <G_cParticleDir TDir>
void G_Particle<TDir>::Update(float time, float total_speed)
{
    if (cur_time == 0.f) UpdateData();
    cur_time += time;
        //  if total speed > 1.f life time must became smaller, if greater larger to save move distance, caurse partical will move faster or slower
    if (cur_time > life_time_cur / total_speed) SetDefaultState();
    else
    {
        static const float seconds_apear = 0.5f;    //  make smooth apperaing of the particle

        float alpha = 0.f;
        if (cur_time < seconds_apear) alpha = cur_time / seconds_apear;     //  particle apear
        else if (cur_time > life_time_cur - seconds_apear) alpha = (life_time_cur - seconds_apear) / seconds_apear; //  particle disappear
        else alpha = 1.f;   //  not transparent

        ZC_Vec3<float> pos((*pPos_cur__last_alpha)[0], (*pPos_cur__last_alpha)[1], (*pPos_cur__last_alpha)[2]); //  last is alpha don't touch alpha
        pos += dir_cur * speed_cur * total_speed * time;
        *pPos_cur__last_alpha = ZC_Vec4<float>(pos, alpha);
    }
}