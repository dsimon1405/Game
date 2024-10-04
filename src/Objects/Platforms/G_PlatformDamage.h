#pragma once

#include <ZC/Events/ZC_EC.h>
#include "G_Platform.h"

class G_PlatformDamage : public G_Platform
{
public:
    G_PlatformDamage(const G_PlatformTransforms& _plat_trans);

private:
    static inline const float seconds_to_dmg = 2.f;
    static inline const ZC_Vec3<float> dmg_color { 1.f, 0.f, 0.f };
    static inline const float deactivate_time = 1.f;

    struct ChangableData
    {
        bool is_active = false;
        float dmg_time = 0.f;
        ZC_Vec3<float> deactivate_color;
        bool is_first_dmg = true;   //  for color manipulations
    } ch_d;

    float damage = 0.f;     //  calculates in ctr

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    void VDeactivatePlatform_P() override;

    void Callback_Updater(float time);
};