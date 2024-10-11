#pragma once

#include <ZC/Events/ZC_EC.h>
#include "G_Platform.h"

class G_PlatformDamage : public G_Platform
{
public:
    G_PlatformDamage(const G_PlatformTransforms& _plat_trans);

private:

    struct ChangableData
    {
        bool is_active = false;
        float dmg_time = 0.f;
        ZC_Vec3<float> deactivate_color;
        float deactivate_sound_load_dmg_volume = 0.f;
        bool is_first_dmg = true;   //  for color manipulations
    } ch_d;

    float damage = 0.f;     //  calculates in ctr

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    void VDeactivatePlatform_P() override;

    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
};