#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DSController.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Model/G_ModelSet.h>

enum G_Object_Type
{
    G_O__Sphere,
    G_O__PlatformQuad,
};

struct G_Object
{
    ZC_DSController dsCon;
    ZC_uptr<ZC_CollisionObject> upCO;

    G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject> _upCO);
    virtual ~G_Object() = default;

    virtual G_Object_Type GetType() const = 0;
    virtual void SetEventAndUpdateState(bool use) {}
};