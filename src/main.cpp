#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/Textured/G_TParticle.h>
int main(int argv, char** args)
{
    G_System system;
    G_GameManager gm;

    // G_TParticle tp(1);

    system.RunMainCycle();
    
    return 0;
}

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system