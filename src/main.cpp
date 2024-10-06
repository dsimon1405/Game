#include "System/G_Window.h"
#include "GamePlay/G_GameManager.h"

int main(int argv, char** args)
{

    G_Window window;
    G_GameManager gm;

    // G_Config::Update_language(G_L_English);
    // G_Config::Update_language(G_L_Russian);

    // ZC_upSound s = ZC_Sounds::GetSound(0);
    // s->Play();

    
    window.RunMainCycle();

    return 0;
}