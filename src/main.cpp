#include "System/G_Window.h"
#include "GamePlay/G_GameManager.h"

#include <ZC/Audio/ZC_Sounds.h>
// #include "System/G_SoundName.h"
#include <ZC/Events/ZC_Events.h>
#include <ZC/Tools/ZC_Random.h>

int main(int argv, char** args)
{
    G_Window window;
    G_GameManager gm;

    // G_Config::Update_language(G_L_English);
    // G_Config::Update_language(G_L_Russian);


    window.RunMainCycle();

    return 0;
}