#pragma once

#include "../G_Object.h"
#include "G_PlatformTransforms.h"
#include <ZC/Events/ZC_EC.h>

enum G_ActivePlatforms
{
    G_AP_Damage,
    G_AP_Disapear,
    G_AP_Scale,
    G_AP_Wind,

    G_AP_Win    //  must be lastfor calculation
};

class G_Platform : public G_Object
{
public:
    // void CallbackTestRotate(float time);

    G_Platform(const G_PlatformTransforms& _plat_trans);
    ~G_Platform();

        //  rotattion around platform origin. angle - on wich need to current angle
    void RotateInternal(float angle);
        //  rotation around map origin. angle - on wich need to current angle
    void RotateExternal(float angle);
    
protected:
    static inline const ZC_Vec3<float> default_lightning { 0.3f, 0.3f, 0.3f };
    static inline const ZC_Vec3<float> start_lightning_color { 1.f, 1.f, 1.f };

    ZC_EC ecUpdater;

    G_PlatformTransforms platf_trans;
    std::list<G_Object*> objects_on_platform;   //  pushable objectes wich had lust collision with that platform

    G_Platform(const G_PlatformTransforms& _plat_trans, G_ModelName modelName, int texSetId);

    G_ObjectTypeMask VGetType_O() const override;
        //  start callback for all platforms
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);
    
    bool IsObjectInPlatformRadiusXY(G_Object* pObj);

private:
    struct ObjAndPlatform
    {
        G_Object* pObj_pushable = nullptr;    //  pushable object collisioned with some platform
        G_Platform* pPlatf = nullptr;         //  platform with wich object collisiond last
    };

    static inline std::list<ObjAndPlatform> objAndPlatforms;  //  objects wich are in platform cylindric XY radius

    virtual void VAddObjectOnPlatform(G_Object* pObj_add);
    virtual void VDeactivatePlatform_P() {}
};


    // static float angle = -50.f;
    // static const float rot_secs = 4.f;
    // ch_d.time += time;
    // if (ch_d.time >= rot_secs)
    // {
    //     time = 0.f;
    //     angle *= -1.f; 
    // }
    // float rot_angle = time * angle / rot_secs;
    // ZC_Vec4<float> pos = (*(this->upCO->GetModelMatrix()))[3];
    // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Rotate(rot_angle, { 0.f, 0.f, 1.f }).Translate({ pos[0], pos[1], pos[2] })); 