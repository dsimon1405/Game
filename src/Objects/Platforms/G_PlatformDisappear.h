#pragma once

#include "G_Platform.h"
#include <ZC/Events/ZC_EC.h>

class G_PlatformDisappear : public G_Platform
{
public:
    G_PlatformDisappear(const G_PlatformTransforms& _plat_trans);

private:
    enum DisappearState
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
        DisappearState disapear_state = DS_start;
        float time = 0.f;
        ZC_Vec3<float> temp_translate;
        ZC_Vec3<float> cur_color;
    } ch_d;

    void VAddObjectOnPlatform_P(G_Object* pObj_add) override;
    G_LightSet GetLightSet_P() override;

    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
};