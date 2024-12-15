#pragma once

#include <System/G_NewV.h>

#ifdef G_NewV
#include <Objects/Particles/New/G_ParticleSystem.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Objects/G_IObject.h>
#include <Sound/G_GameSoundSet.h>

class G_Star : public G_IObject
{
public:
    G_Star();
    ~G_Star();

        //  calls on creation of level with new amount of sections
    void SetNewPosition(float dist_to_star);
        //  return rdius of collision model (larger then drawing)
    float VGetRadius_IO() const noexcept override;
    void VSetDefaultState_IO() override;

private:
    G_ParticleSystem ps_star;
    ZC_EC ec_updater;
    ZC_CollisionObject collision_object;

    std::list<G_IObject*> objects_dmg;

    G_GameSoundSet ss_star;

    float dist_to_center = 0.f;
    float rot_angle_Z = 0.f;
    float cur_angle_Z = 0.f;

    std::vector<G_GameSound> GetSoundsVector();
    
    ZC_Vec3<float> VGetPosition_IO() noexcept override;
    G_ObjectTypeMask VGetTypeMask_IO() const override;
    
    void Callback_Updater(float time);
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);
    void Callback_UBOLightUpdate();
    
    void UpdateDMG(float time);

    void CalculateModelMatrix(float time);
};
#else
#include <Objects/Particles/G_PS_Star.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Objects/G_IObject.h>
#include <Sound/G_GameSoundSet.h>

class G_Star : public G_IObject
{
public:
    G_Star();
    ~G_Star();

        //  calls on creation of level with new amount of sections
    void SetNewPosition(float dist_to_star);
        //  return rdius of collision model (larger then drawing)
    float VGetRadius_IO() const noexcept override;
    void VSetDefaultState_IO() override;

private:
    static inline const float star_scale = 10.f;

    G_PS_Star particles_star;
    ZC_EC ec_updater;
    ZC_CollisionObject collision_object;

    std::list<G_IObject*> objects_dmg;

    G_GameSoundSet ss_star;

    ZC_DSController ds_con;

    float dist_to_center = 0.f;
    float rot_angle_Z = 0.f;
    float cur_angle_XY = 0.f;
    float cur_angle_Z = 0.f;

    std::vector<G_GameSound> GetSoundsVector();
    
    ZC_Vec3<float> VGetPosition_IO() noexcept override;
    G_ObjectTypeMask VGetTypeMask_IO() const override;
    
    void Callback_Updater(float time);
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);
    void Callback_UBOLightUpdate();
    
    void UpdateDMG(float time);

    void CalculateModelMatrix(float time);
};
#endif