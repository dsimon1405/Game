#pragma once

#include <ZC/Audio/ZC_Sound.h>
#include "G_SoundName.h"

#include <list>

class G_SoundsKeeper;

class G_Sound
{
    friend class G_SoundsKeeper;

public:
    G_Sound(G_SoundName _sound_name);
    ~G_Sound();
    G_Sound(G_Sound&&);

    bool operator == (G_SoundName _sound_name) const noexcept;
        //  return true if sound is playing
    bool DistanceToCameraChanged(float dist_to_cam);
        //  - volume - must be in range [0,1]. That range will be used as coef to adaptate max volume of the sound to current camera positoin and recalculate volume.
    void SetVolume(float volume);

        //  stop/start playin all playing sound at current time (made to stop/start game sounds and avoid to stop audio steam for menu). If more then 1 stands 0.
    static void ChangeSoundPlayState(bool on);

private:
    ZC_upSound upSound;
    G_SoundName sound_name;
    float max_volume = 0.f;
    float volume_cur = 0.f;
    float max_distance = 0.f;   //  distance to camera where the upSound will have weakest effect
    float distacne_pos_to_camera_coef = 1.f;    //  (distance from camera to object position) / max_distance

    struct PlayingSound
    {
        ZC_Sound* pSound;
        ZC_SoundState sound_state_restore = ZC_SS__Stop;    //  uses as temp storage on pause. When sets pause for game, here sets actual state from playing objects and nothing from other. When state restores, updates only sounds with => sound_state_restore != ZC_SS__Stop

        void ChangeState(bool on);
    };
    static inline std::list<PlayingSound> playing_sounds;

    void UpdateVolume();
};