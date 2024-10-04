#pragma once

#include "G_Platform.h"
#include <ZC/Events/ZC_EC.h>

class G_PlatformDisapear : public G_Platform
{
public:
        //  level_radius - distance from {0,0,0} to center of farest platfrom
    G_PlatformDisapear(const G_PlatformTransforms& _plat_trans);

private:
    static inline const float seconds_phase = 1.f;
    static inline const float seconds_half_phase = seconds_phase / 2.f;
    static inline const float disapear_phase1_alpha = 0.5f;
    static inline const float disapear_phase2_alpha = 0.f;
    static inline const float disapear_alpha_max = 1.f;
    static inline const ZC_Vec3<float> disapear_color { 0.f, 0.f, 1.f };
    static inline const uint disapear_color_packed = ZC_PackColorFloatToUInt_RGB(disapear_color[0], disapear_color[1], disapear_color[2]);

    enum DisapearState
    {
        DS_start,
        DS_phase1_disapear,
        DS_phase1_apear,
        DS_phase2_disapear,
        DS_phase2_wait,
        DS_phase2_apear
    };

    struct ChangableData
    {
        DisapearState disapear_state = DS_start;
        float time = 0.f;
        ZC_Mat4<float> temp_mat_model;
    } ch_d;

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    //  don't use deactivation, make full disapear cycle

    void Callback_Updater(float time);
};