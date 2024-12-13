#pragma once

#include "G_Platform.h"
#include <System/G_NewV.h>
#ifdef G_NewV
#include <Objects/Particles/New/G_ParticleSystem.h>
#else
#include <Objects/Particles/G_PS_Wind.h>
#endif

class G_PlatformWind : public G_Platform
{
public:
    G_PlatformWind(const G_PlatformTransforms& _plat_trans);
    ~G_PlatformWind();

    static void FreeParticles();    //  calls from G_GameManager::~G_GameManager()

private:
    static inline const float sound_wind_start = 0.f;

    enum WindPhase
    {
        wind_up,
        wind_max,
        wind_down,
        wind_min,
        wind_end
    };

    struct ChangableData
    {
        bool wind_started = false;
        float cur_time = 0.f;
        ZC_Vec3<float> wind_dir_cur;
        float angle_of_new_dir = 0.f;
        float wind_power = 0.f;
        WindPhase wind_phase = wind_up;
        ZC_Vec3<float> deactivate_color;    //  sets on deactivating platform to move from that color to platform default color
        float deactivate_sound_wind = 0.f;
        float deactivate_particles_alpha = 0.f;
    } ch_d;

#ifdef G_NewV
    struct PS_Collision
    {
        G_PlatformWind* pHolder;    //  current owner of the pObj
        G_Object* pObj;
        ul_zc collision_id;
    };

    static inline ZC_uptr<G_ParticleSystem> upPS;
    static inline std::list<PS_Collision> ps_collisions;
    static inline float bubble_sound_secs_cur = 0.f;
#else
    static inline ZC_uptr<G_PS_Wind> upPS_wind;
#endif
    static inline G_PlatformWind* pParticles_holder = nullptr;    //  current holder of the particles

    void VAddObjectOnPlatform_P(G_Object* pObj_add) override;
    void VDeactivatePlatform_P() override;

    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
};