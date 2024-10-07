#include "G_Sound.h"

#include <ZC/Audio/ZC_Sounds.h>

G_Sound::G_Sound(G_SoundName _sound_name)
    : upSound(ZC_Sounds::GetSound(_sound_name)),
    sound_name(_sound_name)
{
    playing_sounds.emplace_back(PlayingSound{ .pSound = upSound.Get() });

    float default_max_distance = 38.f;  //  max distance in cam 49, make it some smoller

    switch (sound_name)
    {
    case G_SN__sphere_move: max_volume = 70.f; max_distance = default_max_distance; break;
    case G_SN__sphere_flight: max_volume = 100.f; max_distance = default_max_distance; break;
    case G_SN__sphere_jump: max_volume = 100.f; max_distance = default_max_distance; break;
    case G_SN__sphere_lands: max_volume = 50.f; max_distance = default_max_distance; break;
    // case : break;
    }
    volume_cur = max_volume;
}

G_Sound::~G_Sound()
{
    if (!upSound) return;
    for (auto iter = playing_sounds.begin(); iter != playing_sounds.end(); ++iter)
    {
        if (iter->pSound == upSound.Get())
        {
            playing_sounds.erase(iter);
            break;
        }
    }
}

G_Sound::G_Sound(G_Sound&& s)
    : upSound(std::move(s.upSound)),
    sound_name(s.sound_name),
    max_volume(s.max_volume),
    volume_cur(s.volume_cur),
    max_distance(s.max_distance)
{}

bool G_Sound::operator == (G_SoundName _sound_name) const noexcept
{
    return sound_name == _sound_name;
}

bool G_Sound::DistanceToCameraChanged(float dist_to_cam)
{
    float dist_coef = dist_to_cam / max_distance;
    distacne_pos_to_camera_coef = dist_coef > 1.f ? 0.f : 1.f - dist_coef;
    UpdateVolume();
    return upSound->GetState() == ZC_SS__Play || upSound->GetState() == ZC_SS__PlayLoop;
}

void G_Sound::SetVolume(float max_vol_coef)
{
    assert(max_vol_coef <= 1.f);
    volume_cur = max_volume * max_vol_coef;
    UpdateVolume();
}

void G_Sound::UpdateVolume()
{
    upSound->SetVolume(volume_cur * distacne_pos_to_camera_coef);
}



    //  G_Sound::PlayingSound

void G_Sound::PlayingSound::ChangeState(bool on)
{
    if (on)
    {
        if (pSound->GetState() == ZC_SS__Play || pSound->GetState() == ZC_SS__PlayLoop)
        {
            sound_state_restore = pSound->GetState();
            pSound->Pause();
        }
    }
    else
    {
        if (sound_state_restore != ZC_SS__Stop)
        {
            sound_state_restore == ZC_SS__Play ? pSound->Play() : pSound->PlayLoop();
            sound_state_restore = ZC_SS__Stop;  //  back ZC_SS__Stop in restore state for next call ChangeState()
        }
    }
}