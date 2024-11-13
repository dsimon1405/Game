#pragma once

#include <ZC/Tools/Math/ZC_Vec4.h>

struct G_ParticleMoveCircle
{
    /*
    - pPos_cur__last_alpha - pointer from G_ParticlesDrawer::particle_positions. Result positoin = vec3(pPos_cur__last_alpha) + _pObject_pos
    - _pColor - pointer from G_ParticlesDrawer::particle_colors.
    - _pObject_pos - pointer on object position. Example G_PS_Wind::start_pos.
    - _rotate_axises - axises of rotation in model matrix.
    - _rotate_speed - angle / second => to roate pPos_cur__last_alpha.
    */
    G_ParticleMoveCircle(ZC_Vec4<float>* _pPos_cur__last_alpha, ZC_Vec3<unsigned char>* _pColor, ZC_Vec3<float>* _pObject_pos, const ZC_Vec3<float>& _rotate_axises,
        float _rotate_speed);
        
    void SetDefaultState();
    void Update(float time, float total_speed);

private:
        //  On restart particle live =>  pPos_cur__last_alpha = start_pos + *pObject_pos;
    ZC_Vec4<float>* pPos_cur__last_alpha;           //  source value in G_Particles::particle_positions. position of object in current life time
    ZC_Vec3<float>* pObject_pos;        //  pointer on object position to recalculate pPos_cur__last_alpha (sets once by owner)
    ZC_Vec4<float> cur_pos;     //  cur position in space of the objext (origin is pObject_pos)
    
    ZC_Vec3<float> rotate_axises;

    float rotate_speed;    //  determine G_PS_Wind or other G_PS
    
    ZC_Vec3<unsigned char>* pColor;     //  source in G_Particles::particle_colors. Determine G_PS_Wind or other G_PS

    void UpdateData();
};