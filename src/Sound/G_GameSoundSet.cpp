#include "G_GameSoundSet.h"

#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Events/ZC_Events.h>
#include <ZC/Tools/Math/ZC_Vec.h>
#include <ZC/Objects/Camera/ZC_Camera.h>

G_GameSoundSet::G_GameSoundSet(std::vector<G_GameSound>&& _sounds, bool _volume_change_on_scroll)
    : sounds(std::move(_sounds)),
    volume_change_on_scroll(_volume_change_on_scroll)
{
    if (volume_change_on_scroll) ecWheelScroll.NewConnection(ZC_Events::ConnectMouseScrollOnceInFrame({ &G_GameSoundSet::MouseWheelScroll, this }));
}

void G_GameSoundSet::SetPosition(const ZC_Vec3<float>* _pPos)
{
    pPos = _pPos;
}

G_GameSoundSet::~G_GameSoundSet()
{
    ecCamCangedPos.Disconnect();
    if (volume_change_on_scroll) ecWheelScroll.Disconnect();
}

void G_GameSoundSet::SetSoundState(G_SoundName sound_name, ZC_SoundState sound_state)
{                              //  if ( sound play or playloop,              not player        no connection yet (for process other sounds) )   =>  connect to change cam pos event
    if (ZC_Find(sounds, sound_name)->SetSoundState(sound_state) && !volume_change_on_scroll && !ecCamCangedPos.IsConnected())
            ecCamCangedPos.NewConnection(ZC_Events::ConnectActiveCameraChangePosition({ &G_GameSoundSet::Callback_CameraChangedPosition, this }));
}

void G_GameSoundSet::SetDefaultState()
{
    dist_to_cam = ZC_Vec::Length(ZC_Camera::GetActiveCamera()->GetPosition() - *pPos);
    for (G_GameSound& sound : sounds) sound.SetDefaultState(dist_to_cam);
    sounds_temp.clear();
}

void G_GameSoundSet::AddTempSound(G_GameSound&& _sound)
{       //  in temp sounds, sounds play only one time, delete stoped sounds before add new
    for (auto iter = sounds_temp.begin(); iter != sounds_temp.end(); )      //  delete stoped 
        iter = iter->GetState() == ZC_SS__Stop ? sounds_temp.erase(iter) : ++iter;

    _sound.DistanceToCameraChanged(dist_to_cam);
    sounds_temp.emplace_back(std::move(_sound)).SetSoundState(ZC_SS__Play);
}

void G_GameSoundSet::SetVolume(G_SoundName sound_name, float volume)
{
    ZC_Find(sounds, sound_name)->SetVolume(volume);
}

float G_GameSoundSet::GetVolume(G_SoundName sound_name)
{
    return ZC_Find(sounds, sound_name)->GetVolume();
}

void G_GameSoundSet::Callback_CameraChangedPosition(const ZC_Vec3<float>& cam_pos)
{
    dist_to_cam = ZC_Vec::Length(cam_pos - *pPos);
    bool have_playing_sound = false;
    for (G_GameSound& sound : sounds)   //  main sounds
    {
        bool is_plaing = sound.DistanceToCameraChanged(dist_to_cam);
        if (!have_playing_sound) have_playing_sound = is_plaing;
    }
    
    for (auto iter = sounds_temp.begin(); iter != sounds_temp.end(); )      //  temp sounds (delete stoped) 
        iter = iter->DistanceToCameraChanged(dist_to_cam) ? ++iter : sounds_temp.erase(iter);

    if (!have_playing_sound) ecCamCangedPos.Disconnect();   //  will be reconnected when some sound start playing in SetSoundState
}

void G_GameSoundSet::MouseWheelScroll(float,float,float)
{
    dist_to_cam = ZC_Vec::Length(ZC_Camera::GetActiveCamera()->GetPosition() - *pPos);
    for (G_GameSound& sound : sounds) sound.DistanceToCameraChanged(dist_to_cam);   //  main sounds
    
    for (auto iter = sounds_temp.begin(); iter != sounds_temp.end(); )      //  temp sounds (delete stoped) 
        iter = iter->DistanceToCameraChanged(dist_to_cam) ? ++iter : sounds_temp.erase(iter);
}