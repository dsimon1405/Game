#pragma once

#include "../G_Object.h"
#include "G_PlatformTransforms.h"
#include <ZC/Events/ZC_EC.h>
#include <ZC/Tools/Math/ZC_Math.h>
#include "G_PlatformLightHolder.h"

enum G_PlatformType
{
    G_PT__Start,
    G_PT__Win,
    
    G_PT__Damage,    //  must be first dangerous platform for calculation G_Section::GetRandomPlatform
    G_PT__Disapear,
    G_PT__Scale,
    G_PT__Wind,      //  must be last dangerous platform for calculation G_Section::GetRandomPlatform
};

class G_Platform : public G_Object
{
public:
    const G_PlatformType platform_type;

    G_Platform(const G_PlatformTransforms& _plat_trans);
    ~G_Platform();

        //  rotattion around platform origin. angle - on wich need to current angle
    void RotateInternal(float angle);
        //  rotation around map origin. angle - on wich need to current angle
    void RotateExternal(float angle);
        //   try to switch win platform position. Return true on success, otherwise wsitching platform is not empty.
    bool SwitchWithWinPlatform(G_Platform* pPLat_win);
        //  calls from G_PlatfromLightHolder::Callback_Updater(). Overrides G_PlatformDisappear, G_PlatformScale.
    virtual G_LightSet GetLightSet_P();
    
protected:
    static inline const ZC_Vec3<float> color_default { 0.f, 0.f, 0.f };
    static inline const uint color_default_packed = ZC_PackColorFloatToUInt_RGB(color_default[0], color_default[1], color_default[2]);
    static inline const ZC_Vec3<float> color_white { 1.f, 1.f, 1.f };
    static inline const uint packed_active_color = ZC_PackColorFloatToUInt_RGB(color_white[0], color_white[1], color_white[2]);
    static inline const float platform_light_radius_divisor = 9.2f;

    ZC_EC ecUpdater;

    G_PlatformTransforms platf_trans;
    std::list<G_Object*> objects_on_platform;   //  pushable objectes wich had lust collision with that platform

    G_Platform(const G_PlatformTransforms& _plat_trans, G_ModelName modelName, int texSetId, ZC_uptr<G_GameSoundSet>&& _upSK, G_PlatformType _platform_type);

    G_ObjectTypeMask VGetTypeMask_IO() const override;
        //  start callback for all platforms
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);
    bool IsObjectInPlatformRadiusXY(G_Object* pObj);
    ZC_Vec3<float> CalculateLightPos(float platform_radius);
        //  erase platform from objects_on_platform and deactivate platfrom in light_holder
    std::list<G_Object*>::iterator EraseObjectFromPlatform(std::list<G_Object*>::iterator iter);

private:
    struct ObjAndPlatform
    {
        G_Object* pObj_pushable = nullptr;    //  pushable object collisioned with some platform
        G_Platform* pPlatf = nullptr;         //  platform with wich object collisiond last
    };

    static inline std::list<ObjAndPlatform> objAndPlatforms;  //  objects wich are in platform cylindric XY radius
    
    static inline G_PlatformLightHolder light_holder;   //  last platform with player

    virtual void VAddObjectOnPlatform_P(G_Object* pObj_add);
    virtual void VDeactivatePlatform_P() {}
};