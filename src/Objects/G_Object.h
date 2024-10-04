#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DSController.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Model/G_ModelSet.h>
#include "G_PushSet.h"

enum G_ObjectType
{
    G_OT__None          = 0,
    G_OT__Pushable      = 1,
    G_OT__Damageable    = 1 << 1,
    G_OT__Ground        = 1 << 2,
    G_OT__Player        = 1 << 3,
};
typedef int G_ObjectTypeMask;

struct G_Object
{
    ZC_DSController dsCon;
    ZC_uptr<ZC_CollisionObject> upCO;
    uint unColor = 0;   //  uniform of adding color in sphere.fs. Pack shema bytes 2x10x10x10
    float unAlpha = 1.f;

    G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO);
    virtual ~G_Object() = default;

    virtual G_ObjectTypeMask VGetType_O() const = 0;
        //  make all object params default
    virtual void VMakeDefaultState_O() {}
        //  push object in direction (must call other objects)
    virtual void VPushObjectInDirection_O(const G_PushSet& move_set) {}
    virtual void VDamagerObject_O(float damage) {}

    virtual ZC_Vec3<float> VGetPosition_O() noexcept;
    virtual void VSetPosition_O(const ZC_Vec3<float>& _position) {}
        //  overrides by movable object for changindg pos while on ground (ground is move and object with it). origin for rotation somewhere on map, angle to rotate around Z axis
    virtual void VOnGroundRotateZ_O(const ZC_Vec3<float>& origin, float angle) {}
};