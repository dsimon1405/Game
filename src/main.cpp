#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/Textured/G_TParticle.h>

#include <ZC/Tools/Time/ZC_Timer.h>
void F(const ZC_Vec3<float>&){}
int main(int argv, char** args)
{
    G_System system;
    // G_GameManager gm;
    

    G_Camera cam_test(ZC_Function<void(const ZC_Vec3<float>&)>{&F}, ZC_Vec3<float>{ 0.f, 0.f, 0.f });
    cam_test.SetConnectionToEvents(true);
    G_TParticle tp(0,0,0,0);


    system.RunMainCycle();
    
    return 0;
}

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system