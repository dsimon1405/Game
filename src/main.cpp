#include "System/G_Window.h"
#include "GamePlay/G_GameManager.h"

#include <ZC/Audio/ZC_Sounds.h>
// #include "System/G_SoundName.h"
#include <ZC/Events/ZC_Events.h>

int main(int argv, char** args)
{
    G_Sound r(G_SoundName::G_SN__sphere_flight);
    G_Window window;
    G_GameManager gm;

    // G_Config::Update_language(G_L_English);
    // G_Config::Update_language(G_L_Russian);


// G_SN__sphere_flight,
//     G_SN__sphere_jump,
//     G_SN__sphere_lands,
    // ZC_upSound s = ZC_Sounds::GetSound(G_SN__sphere_move);
    // s->PlayLoop();
    // auto q = s->GetAudioSet();
    // ZC_AudioSet as(ZC_AudioSet::Stereo, 44100, ZC_AudioSet::S16);
    // assert(q == s);


    
    window.RunMainCycle();

    return 0;
}