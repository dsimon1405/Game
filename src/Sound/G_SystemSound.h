#pragma once

#include <ZC/Audio/ZC_Sound.h>
#include "G_SoundName.h"

#include <list>

class G_SystemSound
{
public:
    ZC_upSound upSound = nullptr;

    G_SystemSound() = default;
    G_SystemSound(G_SoundName sound_name);
    G_SystemSound& operator = (G_SystemSound&&);
    ~G_SystemSound();

    static void UpdateSoundsVolume();

private:
    static inline std::list<ZC_Sound*> system_sounds;
};