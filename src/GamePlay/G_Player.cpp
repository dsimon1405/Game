#include "G_Player.h"

#include <ZC/Events/ZC_Events.h>
#include <GamePlay/G_GameManager.h>

G_Player::G_Player(ZC_uptr<G_ObjPlayable>&& _upObj)
    : upObj(std::move(_upObj)),
    camera({ &G_Player::Callback_CameraRotated, this }),
    gui_w_health(upObj->GetHealth())
{
    upObj->SetPlayersCallback({ &G_Player::CallbackPlayerInfo, this });
    upObj->VSetPosition_O({ 0.f, 0.f, 1.f });  //  move sphere to {0,0,0} position of the bottom on radius 1.f
}

G_Player::~G_Player()
{
    ec_W.Disconnect();
    ec_S.Disconnect();
    ec_A.Disconnect();
    ec_D.Disconnect();
    ec_space.Disconnect();
}

void G_Player::SetEventsState(bool use)
{
    if (use)
    {
        assert(use != ec_W.IsConnected());
        if (use == ec_W.IsConnected()) return;
        ec_W.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_W, { &G_Player::Callback_W,this }));
        ec_S.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_S, { &G_Player::Callback_S,this }));
        ec_A.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_A, { &G_Player::Callback_A,this }));
        ec_D.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_D, { &G_Player::Callback_D,this }));
        ec_space.NewConnection(ZC_Events::ConnectButtonClick(ZC_ButtonID::K_SPACE, { &G_Player::Callback_Space,this }, nullptr));
        camera.SetConnectionToEvents(use);
    }
    else
    {
        assert(use != ec_W.IsConnected());
        if (use == ec_W.IsConnected()) return;
        ec_W.Disconnect();
        ec_S.Disconnect();
        ec_A.Disconnect();
        ec_D.Disconnect();
        ec_space.Disconnect();
        camera.SetConnectionToEvents(use);
    }
}

void G_Player::SetDefaultState()
{
    upObj->VMakeDefaultState_O();
    camera.SetDefaultState();
    gui_w_health.SetDefaultState();
}

void G_Player::Callback_CameraRotated(const ZC_Vec3<float>& cam_pos)
{
    dirs_actual = false;
}

void G_Player::Callback_W(ZC_ButtonID, float time)
{
    RecalculateDirection();
    upObj->VMoveInDirection_OP(dir_front);
}

void G_Player::Callback_S(ZC_ButtonID, float time)
{
    RecalculateDirection();
    upObj->VMoveInDirection_OP(dir_front * -1.f);
}

void G_Player::Callback_A(ZC_ButtonID, float time)
{
    RecalculateDirection();
    upObj->VMoveInDirection_OP(dir_right * -1.f);
}

void G_Player::Callback_D(ZC_ButtonID, float time)
{
    RecalculateDirection();
    upObj->VMoveInDirection_OP(dir_right);
}

void G_Player::Callback_Space(ZC_ButtonID, float time)
{
    upObj->VMakeJump_OP();
}

void G_Player::RecalculateDirection()
{
    if (dirs_actual) return;
    static const ZC_Vec3<float> world_up(0.f,0.f,1.f);

    ZC_Vec3<float> cam_dir_front = ZC_Vec::Normalize(upObj->VGetPosition_O() - camera.cam.GetPosition());
    dir_right = ZC_Vec::Normalize(ZC_Vec::Cross(cam_dir_front, world_up));
    dir_front = ZC_Vec::Normalize(ZC_Vec::Cross(world_up, dir_right));

    dirs_actual = true;
}
#include <iostream>
void G_Player::CallbackPlayerInfo(G_PlayerInfro player_info)
{
    switch (player_info)
    {
    case G_PI__position: camera.SetCameraLookOn(upObj->VGetPosition_O()); break;
    case G_PI__health:
    {
        static float prev_health = 100.f;
        float cur_hp = upObj->GetHealth();
        float minus = prev_health - cur_hp;
        prev_health = cur_hp;
std::cout<<upObj->GetHealth()<<"        - "<<minus<<std::endl;
        gui_w_health.UpdateHealth(cur_hp);
        if (cur_hp <= 0.f)
            G_GameManager::pGM->PlayerDead();
    } break;
    case G_PI__cam_rotate_angle_z: camera.RotateCameraHorizontal(upObj->GetRotateAngle()); break;
    case G_PI__fall: G_GameManager::pGM->PlayerDead();
    }
}