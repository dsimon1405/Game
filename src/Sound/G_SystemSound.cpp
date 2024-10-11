#include "G_SystemSound.h"

#include <ZC/Audio/ZC_Sounds.h>
#include <System/G_Config.h>

G_SystemSound::G_SystemSound(G_SoundName sound_name, float _volume)
    : upSound(ZC_Sounds::GetSound(sound_name)),
    volume(_volume)
{
    system_sounds.emplace_back(this)->upSound->SetVolume(volume * G_Config::GetConfigData().volume_coef);
}

G_SystemSound& G_SystemSound::operator = (G_SystemSound&& ss)
{
    upSound = std::move(ss.upSound);
    volume = ss.volume;
    return *this;
}

G_SystemSound::~G_SystemSound()
{
    for (auto iter = system_sounds.begin(); iter != system_sounds.end(); ++iter)
    {
        if (*iter == this)
        {
            system_sounds.erase(iter);
            break;
        }
    }
}

void G_SystemSound::UpdateSoundsVolume()
{
    for (G_SystemSound* pSistemSound : system_sounds) pSistemSound->upSound->SetVolume(pSistemSound->volume * G_Config::GetConfigData().volume_coef);
}