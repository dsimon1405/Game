#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <iostream>
#include <ZC/Tools/Math/ZC_Vec2.h>
int main(int argv, char** args)
{
    ZC_Vec2 vec1(-3.83f, 2.57f);
    float l = sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2]);

    ZC_Vec3<float> frag_pos(-34.68f, 8.3f, 0.3f);
    ZC_Vec3<float> platf_center(-28.69f, 6.84f, -1.f);
    ZC_Vec3<float> dir_platf_to_frag = frag_pos - platf_center;


    ZC_Vec3<float> light_pos(-32.33f, 7.73f, 2.55f);

    G_System system;
    G_GameManager gm;

    system.RunMainCycle();

    return 0;
}

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system