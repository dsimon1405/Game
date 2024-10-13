#include "G_ParticleMoveCircle.h"

#include <ZC/Tools/Math/ZC_Mat4.h>

G_ParticleMoveCircle::G_ParticleMoveCircle(ZC_Vec4<float>* _pPos_cur__last_alpha, ZC_Vec3<unsigned char>* _pColor, ZC_Vec3<float>* _pObject_pos,
        const ZC_Vec3<float>& _rotate_axises, float _rotate_speed)
    : pPos_cur__last_alpha(_pPos_cur__last_alpha),
    pObject_pos(_pObject_pos),
    cur_pos((*pPos_cur__last_alpha)[0], (*pPos_cur__last_alpha)[1], (*pPos_cur__last_alpha)[2], 1.f),
    rotate_axises(_rotate_axises),
    rotate_speed(_rotate_speed),
    pColor(_pColor)
{}

void G_ParticleMoveCircle::Update(float time, float total_speed)
{       //  rotate start_pos => translate to object pos. Don't touch alpha.
    float rotate_angle = rotate_speed * total_speed * time;
    if (rotate_angle != 0.f) cur_pos = ZC_Mat4<float>(1.f).Rotate(rotate_angle, rotate_axises) * cur_pos;
    *pPos_cur__last_alpha = ZC_Vec4<float>(*pObject_pos + ZC_Vec::Vec4_to_Vec3(cur_pos), (*pPos_cur__last_alpha)[3]);
}