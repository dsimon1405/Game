#pragma once

#include "G_PushSet.h"
#include <ZC/Tools/Math/ZC_Vec3.h>
#include "G_ObjectType.h"

struct G_IObject
{
public:
    G_IObject() {}
    virtual ~G_IObject() = default;

    virtual G_ObjectTypeMask VGetTypeMask_IO() const = 0;
        //  push object in direction (must call other objects)
    virtual void VPushObjectInDirection_IO(const G_PushSet& move_set) {}
    virtual void VDamageObject_IO(float damage, G_ObjectType ot_damager) {}
    virtual float VGetRadius_IO() const noexcept { return 0.f; }
    virtual ZC_Vec3<float> VGetPosition_IO() noexcept = 0;
    virtual void VSetPosition_IO(const ZC_Vec3<float>& _position) {}
        //  overrides by movable object for changindg pos while on ground (ground is move and object with it). origin for rotation somewhere on map, angle to rotate around Z axis
    virtual void VOnGroundRotateZ_IO(const ZC_Vec3<float>& origin, float angle) {}
    virtual void VSetDefaultState_IO() {}
};