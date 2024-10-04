#pragma once

#include "G_Platform.h"
#include <ZC/Events/ZC_EC.h>

#include <GamePlay/G_Map.h>

class G_PlatformScale : public G_Platform
{
public:
    G_PlatformScale(const G_PlatformTransforms& _plat_trans);
    
private:
    static inline const float seconds_pause = 1.f;
    static inline const float seconds_scale = 2.f;
    static inline const float scale_min = G_Map::scaleXY_other_platforms * 0.2f;
    static inline const float scale_max = G_Map::scaleXY_other_platforms * 1.f;
    static inline const float scale_Z = G_Map::scaleZ_platform;
    static inline const ZC_Vec3<float> scale_color { 1.f, 1.f, 0.f };
    static inline const uint scale_color_packed = ZC_PackColorFloatToUInt_RGB(scale_color[0], scale_color[1], scale_color[2]);
    static inline const float deactivate_time = 1.f;

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

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    
    void Callback_Updater(float time);
    void CheckPlatformHabitability(float scale_val);
};