#include "G_ObjPlayable.h"

G_ObjPlayable::G_ObjPlayable(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO, float health, ZC_uptr<G_SoundsKeeper>&& _upSK)
    : G_Object(modelName, texSetId, std::move(_upCO), std::move(_upSK)),
    changable_data_op{ .health = health },
    max_hp(health)
{}

G_ObjPlayable::~G_ObjPlayable()
{
    ecUpdater.Disconnect();
}

ZC_Vec3<float> G_ObjPlayable::VGetPosition_O() noexcept
{
    return changable_data_op.position;
}

void G_ObjPlayable::VSetPosition_O(const ZC_Vec3<float>& _position)
{
    changable_data_op.position = _position;
    if (callback_player_info) callback_player_info(G_PI__position);
}

void G_ObjPlayable::VDamagerObject_O(float damage)
{
    changable_data_op.health -= damage;
    VDamageObject_OP(damage);
    if (callback_player_info) callback_player_info(G_PI__health);
}

void G_ObjPlayable::VMakeDefaultState_O()
{
    changable_data_op = { .health = max_hp };
    VMakeDefaultState_OP();
}

void G_ObjPlayable::SetPlayersCallback(ZC_Function<void(G_PlayerInfro)>&& _callback_player_info)
{
    callback_player_info = std::move(_callback_player_info);
    object_type = callback_player_info ? G_OT__Player : G_OT__None;
}

float G_ObjPlayable::GetHealth()
{
    return changable_data_op.health;
}

float G_ObjPlayable::GetRotateAngle()
{
    return changable_data_op.rotate_angle_z;
}