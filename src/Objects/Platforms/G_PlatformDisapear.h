#pragma once

#include "G_Platform.h"
#include <ZC/Events/ZC_EC.h>

class G_PlatformDisapear : public G_Platform
{
public:
        //  level_radius - distance from {0,0,0} to center of farest platfrom
    G_PlatformDisapear(const G_PlatformTransforms& _plat_trans);

private:

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
        ZC_Vec3<float> temp_translate;
        // ZC_Mat4<float> temp_mat_model;
    } ch_d;

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    //  don't use deactivation, make full disapear cycle

    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
};