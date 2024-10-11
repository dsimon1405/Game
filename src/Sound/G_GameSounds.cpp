#include "G_GameSounds.h"

#include "G_GameSound.h"

void G_GameSounds::ChangeSoundsPlayState(bool on)
{
    if (on == game_play_sounds_state) return;

    for (GameSound& ps : game_sounds) ps.ChangeState(on);
    game_play_sounds_state = on;
}

void G_GameSounds::UpdateSoundsVolume()
{
    for (GameSound& s : game_sounds) s.pSound->UpdateVolume(); 
}

void G_GameSounds::AddSound(G_GameSound* pGS)
{
    game_sounds.emplace_back(GameSound{ .pSound = pGS });
}

void G_GameSounds::EraseSound(G_GameSound* pGS)
{
    for (auto iter = game_sounds.begin(); iter != game_sounds.end(); ++iter)
    {
        if (iter->pSound == pGS)
        {
            game_sounds.erase(iter);
            break;
        }
    }
}

void G_GameSounds::SetDefaultSate()
{
    for (GameSound& gs : game_sounds) gs.sound_state_restore = ZC_SS__Stop;
    game_play_sounds_state = true;
}



    //  G_GameSound::GameSound

void G_GameSounds::GameSound::ChangeState(bool on)
{
    if (on)
    {
        if (sound_state_restore != ZC_SS__Stop)
        {
            sound_state_restore == ZC_SS__Play ? pSound->upSound->Play() : pSound->upSound->PlayLoop();
            sound_state_restore = ZC_SS__Stop;  //  back ZC_SS__Stop in restore state for next call ChangeState()
        }
    }
    else
    {
        if (pSound->upSound->GetState() == ZC_SS__Play || pSound->upSound->GetState() == ZC_SS__PlayLoop)
        {
            sound_state_restore = pSound->upSound->GetState();
            pSound->upSound->Pause();
        }
    }
}