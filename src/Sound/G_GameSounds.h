#pragma once

#include "G_GameSound.h"

class G_GameSounds
{
    friend class G_GameSound;
public:
        //  stop/start playin all playing sound at current time (made to stop/start game sounds and avoid to stop audio steam for menu). If more then 1 stands 0.
    static void ChangeSoundsPlayState(bool on);
        //  calls from G_Config:: on users update volume
    static void UpdateSoundsVolume();
        //  don't sets to default state all sounds (bad idea but now. so...)! Set only static data to GameSound::sound_state_restore = ZC_SS__Stop and game_play_sounds_state = true. To have default behaviour on start.
    static void SetDefaultSate();
    
private:
    struct GameSound
    {
        G_GameSound* pSound;
        ZC_SoundState sound_state_restore = ZC_SS__Stop;    //  uses as temp storage on pause. When sets pause for game, here sets actual state from playing objects and nothing from other. When state restores, updates only sounds with => sound_state_restore != ZC_SS__Stop

        void ChangeState(bool on);
    };
    static inline std::list<GameSound> game_sounds;
    static inline bool game_play_sounds_state = true;

    static void AddSound(G_GameSound* pGS);
    static void EraseSound(G_GameSound* pGS);
};