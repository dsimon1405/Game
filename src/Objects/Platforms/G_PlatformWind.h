#pragma once

#include "G_Platform.h"

class G_PlatformWind : public G_Platform
{
public:
    G_PlatformWind(const G_PlatformTransforms& _plat_trans);

private:
    enum WindPhase
    {
        wind_up,
        wind_max,
        wind_down,
        wind_min,
        wind_end
    };
    static inline const float seconds_to_start_wind = 2.f;
    static inline const float seconds_on_one_windPhase = 1.f;
    static inline const float min_wind_power = 0.4f;
    static inline const float max_wind_power = 0.8f;
    static inline const float seconds_deactivate = 1.f;

    struct ChangableData
    {
        bool wind_started = false;
        float wind_time = 0.f;
        ZC_Vec3<float> wind_dir_cur;
        float angle_to_new_dir = 0.f;
        float wind_power = 0.f;
        WindPhase wind_phase = wind_up;
        ZC_Vec3<float> deactivate_color;    //  sets on deactivating platform to move from that color to platform default color
    } ch_d;


    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    void VDeactivatePlatform_P() override;

    void Callback_Updater(float time);
};