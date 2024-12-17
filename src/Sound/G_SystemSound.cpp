#include "G_SystemSound.h"

#include <ZC/Audio/ZC_Sounds.h>
#include <System/G_Config.h>

G_SystemSound::G_SystemSound(G_SoundName sound_name, float _volume)
    : sound(ZC_Sounds::GetSound(sound_name)),
    volume(_volume)
{
    system_sounds.emplace_back(this)->sound.SetVolume(volume * G_Config::GetConfigData().volume_coef);
}

G_SystemSound& G_SystemSound::operator = (G_SystemSound&& ss)
{
    sound = std::move(ss.sound);
    volume = ss.volume;
    system_sounds.emplace_back(this)->sound.SetVolume(volume * G_Config::GetConfigData().volume_coef);
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
    for (G_SystemSound* pSistemSound : system_sounds) pSistemSound->sound.SetVolume(pSistemSound->volume * G_Config::GetConfigData().volume_coef);
}