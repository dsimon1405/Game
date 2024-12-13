#include "G_PS_Source.h"

#include <ZC/Tools/ZC_Random.h> 

bool G_PS_Source::Rotate::operator == (const Rotate& r) const noexcept
{
    return angle_use == r.angle_use && angle_1 == r.angle_1 && angle_2 == r.angle_2;
}

float G_PS_Source::Rotate::GetAngle()
{
    switch (angle_use)
    {
    case AU__contant: return angle_1;
    case AU__random_between_constants: return ZC_Random::GetRandomInt(0, 1) ? angle_1 : angle_2;
    case AU__random_between_angles: return ZC_Random::GetRandomFloat_x_100(angle_1, angle_2);
    }
    return 0.f;
}


    //  G_PS_Source::Animation

float G_PS_Source::Animation::Calc_animation_start_secs(float life_time_secs_total)
{
    return offset_from == G_PS_Source::Animation::OF__Start ? offset_to_start_animation_secs : life_time_secs_total - offset_to_start_animation_secs;
}