#include "G_GameSound.h"

#include <ZC/Audio/ZC_Sounds.h>
#include <System/G_Config.h>
#include <GamePlay/G_Map.h>

G_GameSound::G_GameSound(G_SoundName _sound_name)
    : upSound(ZC_Sounds::GetSound(_sound_name)),
    sound_name(_sound_name)
{
                    //      volume_max(100)       volume_default       distance_max(not 0)        distance_min
    
    static const float max_dist_to_cam = 40.f;  //  max distance in G_Camera 49
    static const float min_dist_to_cam = 15.f;  //  max distance in G_Camera 15

    // static const float platform_sound_radius_min = G_Map::scaleXY_other_platforms * G_Map::platforms_model_radius;
    // static const float platform_sound_radius_max = platform_sound_radius_min * 2.f;

    switch (sound_name)
    {
        //  objects
            //  sphere
    case G_SN__sphere_move: volume_max = 30.f; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; sound_state_default = ZC_SS__PlayLoop; break;
    case G_SN__sphere_flight: volume_max = 100.f; volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;
    case G_SN__sphere_jump: volume_max = 100.f; volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;
    case G_SN__sphere_lands: volume_max = 30.f; volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;
    case G_SN__sphere_dmg_1: volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;
    case G_SN__sphere_dmg_2: volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;

            //  platform
    case G_SN__platform_activation: volume_default = volume_max; distance_min = min_dist_to_cam; distance_max = max_dist_to_cam; break;

        //  gui
    case G_SN__gui_start_timer: volume_max = 100.f; volume_default = volume_max; distance_min = 0.f; distance_max = max_dist_to_cam; break;
            //  win
    case G_SN__gui_win_sound: volume_default = volume_max; distance_max = 1.f; break;
    case G_SN__gui_win_transfer_time_points: volume_default = volume_max; distance_max = 1.f; break;
    default: assert(false);
    }

    distance_max -= distance_min;   //  calculate profit to calculate volume radius, all in distance min have max volume
    volume_cur = volume_default;
    UpdateVolume();

    all_game_sounds.emplace_back(Sound{ .pSound = this });
}

G_GameSound::G_GameSound(G_GameSound&& s)
    : upSound(std::move(s.upSound)),
    sound_name(s.sound_name),
    sound_state_default(s.sound_state_default),
    volume_max(s.volume_max),
    volume_default(s.volume_default),
    volume_cur(s.volume_cur),
    distance_max(s.distance_max),
    distance_min(s.distance_min),
    distacne_pos_to_camera_coef(s.distacne_pos_to_camera_coef)
{
    all_game_sounds.emplace_back(Sound{ .pSound = this });
}

G_GameSound::~G_GameSound()
{
    for (auto iter = all_game_sounds.begin(); iter != all_game_sounds.end(); ++iter)
    {
        if (iter->pSound == this)
        {
            all_game_sounds.erase(iter);
            break;
        }
    }
}

bool G_GameSound::operator == (G_SoundName _sound_name) const noexcept
{
    return sound_name == _sound_name;
}

bool G_GameSound::DistanceToCameraChanged(float dist_to_cam)
{
    dist_to_cam -= distance_min;    //  move cam distance to min radius (min radius have max volume, calculation need start from distance_min), distance_max recalculated in ctr
    assert(distance_max != 0);
    float dist_coef = dist_to_cam / distance_max;
    distacne_pos_to_camera_coef = dist_coef > 1.f ? 0.f     //  cam too far
                                : dist_coef <= 0.f ? 1.f    //  cam inside radius of max volume
                                : 1.f - dist_coef;          //  cam somewhere in volume radius
    distacne_pos_to_camera_coef = dist_coef > 1.f ? 0.f : 1.f - dist_coef;
    UpdateVolume();
    return upSound->GetState() == ZC_SS__Play || upSound->GetState() == ZC_SS__PlayLoop;
}

void G_GameSound::SetVolume(float volume)
{
    assert(volume <= 1.f);
    volume_cur = volume_max * volume;
    UpdateVolume();
}

bool G_GameSound::SetDefaultState(float dist_to_cam)
{
    volume_cur = volume_default;
    DistanceToCameraChanged(dist_to_cam);
    return SetSoundState(sound_state_default);
}

void G_GameSound::UpdateVolume()
{
    upSound->SetVolume(volume_cur * distacne_pos_to_camera_coef * G_Config::GetConfigData().volume_coef);
}

bool G_GameSound::SetSoundState(ZC_SoundState sound_state)
{
    switch (sound_state)
    {
    case ZC_SS__Play: upSound->Play(); return true;
    case ZC_SS__PlayLoop: upSound->PlayLoop(); return true;
    case ZC_SS__Pause: upSound->Pause(); return false;
    case ZC_SS__Stop: upSound->Stop(); return false;
    }
    return false;
}

ZC_SoundState G_GameSound::GetState()
{
    return upSound->GetState();
}

void G_GameSound::ChangeSoundsPlayState(bool on)
{
    static bool game_play_sounds_state = true;
    if (on == game_play_sounds_state) return;

    for (Sound& ps : all_game_sounds) ps.ChangeState(on);
    game_play_sounds_state = on;
}

void G_GameSound::UpdateSoundsVolume()
{
    for (Sound& s : all_game_sounds) s.pSound->UpdateVolume(); 
}


    //  G_GameSound::Sound

void G_GameSound::Sound::ChangeState(bool on)
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