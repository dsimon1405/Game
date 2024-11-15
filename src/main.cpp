#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/Textured/G_TParticle.h>
int main(int argv, char** args)
{
    // ZC_Vec2<float> corners[]
    // {
    //     { -1.f, -1.f },     //  bl
    //     {  1.f, -1.f },     //  br
    //     {  1.f,  1.f },     //  tr
    //     { -1.f,  1.f }      //  tl
    // };
    // ZC_Vec3<float> dir_right = ZC_Vec::Normalize<float>({ 1.f, 0.f, 0.f });
    // ZC_Vec3<float> dir_up = ZC_Vec::Normalize<float>({ 0.f, 0.f, 1.f });
    // float half_width = 1.f;
    // float half_height = 2.f;

    // auto calc_corn = [](ZC_Vec3<float> dir, float corner, float size)
    // {
    //     return dir * corner * size;
    // };

    // ZC_Vec3<float> pos(2.f, 2.f, 2.f);

    // ZC_Vec3<float> bl = pos + (calc_corn(dir_right, corners[0][0], half_width) + calc_corn(dir_up, corners[0][1], half_height));
    // ZC_Vec3<float> br = pos + (calc_corn(dir_right, corners[1][0], half_width) + calc_corn(dir_up, corners[1][1], half_height));
    // ZC_Vec3<float> tr = pos + (calc_corn(dir_right, corners[2][0], half_width) + calc_corn(dir_up, corners[2][1], half_height));
    // ZC_Vec3<float> tl = pos + (calc_corn(dir_right, corners[3][0], half_width) + calc_corn(dir_up, corners[3][1], half_height));


    G_System system;
    G_GameManager gm;

    G_TParticle tp(1);

    system.RunMainCycle();
    
    return 0;
}

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system