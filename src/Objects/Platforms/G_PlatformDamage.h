#pragma once

#include <ZC/Events/ZC_EC.h>
#include "G_Platform.h"
#include <Objects/G_CylinderDMG.h>

class G_PlatformDamage : public G_Platform
{
public:
    G_PlatformDamage(const G_PlatformTransforms& _plat_trans);
    ~G_PlatformDamage();

    static void FreeCylinderDMG();    //  calls from G_GameManager::~G_GameManager()

private:
    struct ChangableData
    {
        bool is_active = false;
        float dmg_time = 0.f;
        ZC_Vec3<float> deactivate_color;
        float deactivate_sound_load_dmg_volume = 0.f;
        bool is_first_dmg = true;   //  for color manipulations
        float cylinder_end_time = 0.f;
    } ch_d;

    float damage = 0.f;     //  calculates in ctr

    static inline ZC_uptr<G_CylinderDMG> upCylinder_dmg;
    static inline G_PlatformDamage* pPlatf_cylinder_holder;

    void VAddObjectOnPlatform_P(G_Object* pObj_add) override;
    void VDeactivatePlatform_P() override;

    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
    void UpdateCylinderDMG(float cylinder_time);
};