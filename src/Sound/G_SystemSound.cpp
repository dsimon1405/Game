#include "G_SystemSound.h"

#include <ZC/Audio/ZC_Sounds.h>
#include <System/G_Config.h>

G_SystemSound::G_SystemSound(G_SoundName sound_name)
    : upSound(ZC_Sounds::GetSound(sound_name))
{
    system_sounds.emplace_back(upSound.Get());
}

G_SystemSound& G_SystemSound::operator = (G_SystemSound&& ss)
{
    upSound = std::move(ss.upSound);
    return *this;
}

G_SystemSound::~G_SystemSound()
{
    if (!upSound) return;
    for (auto iter = system_sounds.begin(); iter != system_sounds.end(); ++iter)
    {
        if (*iter == upSound.Get())
        {
            system_sounds.erase(iter);
            break;
        }
    }
}

void G_SystemSound::UpdateSoundsVolume()
{
    for (ZC_Sound* pSound : system_sounds) pSound->SetVolume(G_Config::GetConfigData().volume_coef * 100.f);
}