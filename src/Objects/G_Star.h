#pragma once

#include <Objects/Particles/G_PS_Star.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Objects/G_IObject.h>
#include <ZC/Video/OpenGL/Buffer/ZC_UBO.h>

class G_Star : public G_IObject
{
public:
    G_Star();
    ~G_Star();

        //  calls on creation of level with new amount of sections
    void SetNewPosition(float dist_to_star);
        //  return rdius of collision model (larger then drawing)
    float VGetRadius_IO() const noexcept override;

private:
    G_PS_Star particles_star;
    ZC_EC ec_updater;
    ZC_CollisionObject collision_object;

    std::list<G_IObject*> objects_dmg;

    ZC_UBO ubo_light;
    
    ZC_Vec3<float> VGetPosition_IO() noexcept override;
    G_ObjectTypeMask VGetTypeMask_IO() const override;
    
    void Callback_Updater(float time);
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);
    
    void UpdateDMG(float time);
};