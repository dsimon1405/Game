#pragma once

#include "G_GameSound.h"
#include <ZC/Events/ZC_EC.h>
#include <ZC/Tools/Function/ZC_Function.h>
#include <ZC/Tools/Math/ZC_Vec3.h>

#include <vector>
#include <list>

class G_GameSoundSet
{
public:
        //  _volume_change_on_scroll only for player
    G_GameSoundSet(std::vector<G_GameSound>&& _sounds, bool _volume_change_on_scroll = false);
    ~G_GameSoundSet();

    void SetPosition(const ZC_Vec3<float>* _pPos);
    void SetSoundState(G_SoundName sound_name, ZC_SoundState sound_state);
        //  - volume - must be in range [0,1]. That range will be used as coef to adaptate max volume of the sound to current camera positoin and recalculate volume.
    void SetVolume(G_SoundName sound_name, float volume);
    float GetVolume(G_SoundName sound_name);
        //  set default volume for all sounds with request of camera postion. Delete sounds added with AddTempSound().
    void SetDefaultState();
        //  add sound wich will play one time and then delete
    void AddTempSound(G_GameSound&& _sound);

private:
    const ZC_Vec3<float>* pPos;
    std::vector<G_GameSound> sounds;
    std::list<G_GameSound> sounds_temp;
    ZC_EC ecCamCangedPos;
        //  data for player
    static inline ZC_EC ecWheelScroll;
    bool volume_change_on_scroll;
    float dist_to_cam = 0.f;    //  store last calculated distance to came

    void Callback_CameraChangedPosition(const ZC_Vec3<float>& cam_pos);
        //  uses for player's object, need change volume only and check sound only on cahnging cam pos with scroll event
    void MouseWheelScroll(float,float,float);
};
