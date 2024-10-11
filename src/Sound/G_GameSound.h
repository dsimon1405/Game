#pragma once

#include <ZC/Audio/ZC_Sound.h>
#include "G_SoundName.h"

#include <list>

class G_GameSounds;

class G_GameSound
{
    friend class G_GameSounds;
public:
    G_GameSound() = delete;
    G_GameSound(G_SoundName _sound_name);
    G_GameSound(const G_GameSound&) = delete;
    G_GameSound(G_GameSound&& gs);
    ~G_GameSound();

    bool operator == (G_SoundName _sound_name) const noexcept;
        //  return true if sound is playing
    bool DistanceToCameraChanged(float dist_to_cam);
        //  - volume - must be in range [0,1]. That range will be used as coef to adaptate max volume of the sound to current camera positoin and recalculate volume.
    void SetVolume(float volume);
    float GetVolume() const noexcept;
        //  set default volume with recalculated distance to camera coef. Return true if default state ZC_SS_Play or ZC_SS_PlayLoop
    bool SetDefaultState(float dist_to_cam);
    bool SetSoundState(ZC_SoundState sound_state);
    ZC_SoundState GetState();

private:
    ZC_upSound upSound;
    G_SoundName sound_name;

    ZC_SoundState sound_state_default = ZC_SS__Stop;

    float volume_max = 100.f;
    float volume_default = 0.f; //  sets in ctr if need not 0
    float volume_cur = 0.f;

    float distance_max = 0.f;   //  distance from object pos to camera (large radius) where sound will have weakest effect
    float distance_min = 0.f;   //  distance from object pos to camera (mini radius) where sound will have max effect
    float distacne_pos_to_camera_coef = 1.f;    //  (distance from camera to object position) / distance_max

    void UpdateVolume();
};