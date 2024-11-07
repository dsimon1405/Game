#pragma once

#include "G_Platform.h"
#include <ZC/Events/ZC_EC.h>
#include <GamePlay/G_Map.h>

class G_PlatformScale : public G_Platform
{
public:
    G_PlatformScale(const G_PlatformTransforms& _plat_trans);
    
private:
    static inline const float scale_xy_min = G_Map::scaleXY_other_platforms * 0.2f;
    static inline const float scale_xy_max = G_Map::scaleXY_other_platforms;

    enum ScaleState
    {
        SS_start,
        SS_scale_down,
        SS_pause_on_down,
        SS_scale_up,
        SS_pause_on_up,
        SS_end
    };

    struct ChangableData
    {
        ScaleState scale_state = SS_start;
        float scale_time = 0.f;
        float deactivate_scale = 0.f;
    } ch_d;

    void VAddObjectOnPlatform_P(G_Object* pObj_add) override;
    G_LightSet GetLightSet_P() override;
    
    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
    void CheckPlatformHabitability(float scale_val);
};