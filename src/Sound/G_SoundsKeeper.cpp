#include "G_SoundsKeeper.h"

#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Events/ZC_Events.h>
#include <ZC/Tools/Math/ZC_Vec.h>
#include <ZC/Objects/Camera/ZC_Camera.h>

G_SoundsKeeper::G_SoundsKeeper(std::vector<G_Sound>&& _sounds, bool _volume_change_on_scroll)
    : sounds(std::move(_sounds)),
    volume_change_on_scroll(_volume_change_on_scroll)
{
    if (volume_change_on_scroll) ecWheelScroll.NewConnection(ZC_Events::ConnectMouseScrollOnceInFrame({ &G_SoundsKeeper::MouseWheelScroll, this }));
}

void G_SoundsKeeper::SetPosition(const ZC_Vec3<float>* _pPos)
{
    pPos = _pPos;
}

G_SoundsKeeper::~G_SoundsKeeper()
{
    ecCamCangedPos.Disconnect();
    if (volume_change_on_scroll) ecWheelScroll.Disconnect();
}

void G_SoundsKeeper::SetSoundState(G_SoundName sound_name, ZC_SoundState sound_state)
{
    G_Sound* pSound = ZC_Find(sounds, sound_name);
    assert(pSound);
    switch (sound_state)
    {
    case ZC_SS__Play:
    {
        pSound->upSound->Play();
        if (!volume_change_on_scroll && !ecCamCangedPos.IsConnected())
            ecCamCangedPos.NewConnection(ZC_Events::ConnectActiveCameraChangePosition({ &G_SoundsKeeper::Callback_CameraChangedPosition, this }));
    } break;
    case ZC_SS__PlayLoop:
    {
        pSound->upSound->PlayLoop();
        if (!volume_change_on_scroll && !ecCamCangedPos.IsConnected())
            ecCamCangedPos.NewConnection(ZC_Events::ConnectActiveCameraChangePosition({ &G_SoundsKeeper::Callback_CameraChangedPosition, this }));
    } break;
    case ZC_SS__Pause: pSound->upSound->Pause(); break;
    case ZC_SS__Stop: pSound->upSound->Stop(); break;
    }
}

void G_SoundsKeeper::SetDefault()
{
    for (G_Sound& sound : sounds) sound.upSound->Stop();
}

void G_SoundsKeeper::GetVolume(G_SoundName sound_name, float& rVol_cur, float& rVol_max_system)
{
    G_Sound* pSound = ZC_Find(sounds, sound_name);
    rVol_cur = pSound->upSound->GetVolume();
    rVol_max_system = pSound->max_volume;
}

void G_SoundsKeeper::SetVolume(G_SoundName sound_name, float volume)
{
    ZC_Find(sounds, sound_name)->SetVolume(volume);
}

void G_SoundsKeeper::Callback_CameraChangedPosition(const ZC_Vec3<float>& cam_pos)
{
    float dist_to_cam = ZC_Vec::Length(cam_pos - *pPos);
    bool have_playing_sound = false;
    for (G_Sound& sound : sounds)
    {
        bool is_plaing = sound.DistanceToCameraChanged(dist_to_cam);
        if (!have_playing_sound) have_playing_sound = is_plaing;
    }
    if (!have_playing_sound) ecCamCangedPos.Disconnect();   //  will be reconnected when some sound start playing in SetSoundState
}

void G_SoundsKeeper::MouseWheelScroll(float,float,float)
{
    float dist_to_cam = ZC_Vec::Length(ZC_Camera::GetActiveCamera()->GetPosition() - *pPos);
    for (G_Sound& sound : sounds) sound.DistanceToCameraChanged(dist_to_cam);
}