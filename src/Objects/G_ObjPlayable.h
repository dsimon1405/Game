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
    G_ObjPlayable(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO, float health, ZC_uptr<G_GameSoundSet>&& _upSK);
    ~G_ObjPlayable();

        //  calls by owner (player...)
    virtual void VMoveInDirection_OP(const ZC_Vec3<float>& dir) = 0;
    virtual void VMakeJump_OP() = 0;

    ZC_Vec3<float> VGetPosition_IO() noexcept override;
    void VSetPosition_IO(const ZC_Vec3<float>& _position) override;
    void VDamageObject_IO(float damage, G_ObjectType ot_damager) override;

    void SetPlayersCallback(ZC_Function<void(G_PlayerInfro)>&& _callback_player_info);
    float GetHealth();
    float GetRotateAngle();
    G_ObjectType GetTypeOfLastDamager();

protected:
    struct ChangableData_OP
    {
        float health = 0.f;
        ZC_Vec3<float> position;
        float rotate_angle_z = 0.f;
        G_ObjectType ot_last_damager = G_OT__None;
    } changable_data_op;

    ZC_EC ecUpdater;
    G_ObjectType object_type = G_OT__None;
    const float max_hp;

    ZC_Function<void(G_PlayerInfro)> callback_player_info;
    
    void VSetDefaultState_O() override;

    virtual void VSetDefaultState_OP() = 0;
    virtual void VDamageObject_OP(float damage, G_ObjectType ot_damager) {}
};