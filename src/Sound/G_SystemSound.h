#pragma once

#include <ZC/Audio/ZC_Sound.h>
#include "G_SoundName.h"

#include <list>

class G_SystemSound
{
public:
    ZC_upSound upSound = nullptr;
        //  Look G_Button. May not have sound.
    G_SystemSound() = default;
        //  - _volume - range [0, 100]. Param for updating global sound volume from G_Config in UpdateSoundsVolume(). Set once, and must not be changed in upSound!
    G_SystemSound(G_SoundName sound_name, float _volume);
    G_SystemSound& operator = (G_SystemSound&&);
    ~G_SystemSound();

    static void UpdateSoundsVolume();

private:
    static inline std::list<G_SystemSound*> system_sounds;

    float volume;
};