#pragma once

#include "G_Object.h"
#include <ZC/Events/ZC_EC.h>

//  information from playable object to player about object's state changes
enum G_PlayerInfro
{
    G_PI__position,
    G_PI__health,
    G_PI__cam_rotate_angle_z,   //  some game objects changed camera rotation Z on some angle
    G_PI__fall,     //  fall from platform
};

class G_ObjPlayable : public G_Object
{
public:
    G_ObjPlayable(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO);
    ~G_ObjPlayable();

        //  calls by owner (player...)
    virtual void VMoveInDirection_OP(const ZC_Vec3<float>& dir) = 0;
    virtual void VMakeJump_OP() = 0;

    ZC_Vec3<float> VGetPosition_O() noexcept override;
    void VSetPosition_O(const ZC_Vec3<float>& _position) override;
    void VDamagerObject_O(float damage) override;
    void VMakeDefaultState_O() override;

    void SetPlayersCallback(ZC_Function<void(G_PlayerInfro)>&& _callback_player_info);
    float GetHealth();
    float GetRotateAngle();

protected:
    struct ChangableData_OP
    {
        float health = 100.f;
        ZC_Vec3<float> position;
        float rotate_angle_z = 0.f;
    } changable_data_op;

    ZC_EC ecUpdater;
    G_ObjectType object_type = G_OT__None;

    ZC_Function<void(G_PlayerInfro)> callback_player_info;

    virtual void VMakeDefaultState_OP() = 0;
};