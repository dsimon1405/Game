#pragma once

#include "G_Sound.h"
#include <ZC/Events/ZC_EC.h>
#include <ZC/Tools/Function/ZC_Function.h>
#include <ZC/Tools/Math/ZC_Vec3.h>

#include <vector>

class G_SoundsKeeper
{
public:
        //  _volume_change_on_scroll only for player
    G_SoundsKeeper(std::vector<G_Sound>&& _sounds, bool _volume_change_on_scroll = false);
    ~G_SoundsKeeper();

    void SetPosition(const ZC_Vec3<float>* _pPos);
    void SetSoundState(G_SoundName sound_name, ZC_SoundState sound_state);
    void SetDefault();
    void GetVolume(G_SoundName sound_name, float& rVol_cur, float& rVol_max_system);
        //  - volume - must be in range [0,1]. That range will be used as coef to adaptate max volume of the sound to current camera positoin and recalculate volume.
    void SetVolume(G_SoundName sound_name, float volume);

private:
    const ZC_Vec3<float>* pPos;
    std::vector<G_Sound> sounds;
    ZC_EC ecCamCangedPos;
        //  data for player
    static inline ZC_EC ecWheelScroll;
    bool volume_change_on_scroll;

    void Callback_CameraChangedPosition(const ZC_Vec3<float>& cam_pos);
        //  uses for player's object, need change volume only and check sound only on cahnging cam pos with scroll event
    void MouseWheelScroll(float,float,float);
};
