#include "G_Player.h"

#include <ZC/Events/ZC_Events.h>
#include <GamePlay/G_GameManager.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <Objects/G_OP_MarbleSphere.h>

G_Player::G_Player()
    : camera({ &G_Player::Callback_CameraRotated, this }, { 0.f, 0.f, 1.f }),
    upObj(new G_OP_MarbleSphere(true)),
    gui_w_health(upObj->GetHealth())
{
    upObj->SetPlayersCallback({ &G_Player::CallbackPlayerInfo, this });
    upObj->VSetPosition_IO({ 0.f, 0.f, upObj->GetRadius() });  //  move sphere to {0,0,0} position of the bottom on radius 1.f
}

G_Player::~G_Player()
{
    ec_W.Disconnect();
    ec_S.Disconnect();
    ec_A.Disconnect();
    ec_D.Disconnect();
    ec_space.Disconnect();
    ec_updater.Disconnect();
}

void G_Player::ChangeActivityState(bool on)
{
    ChangeMoveState(on);
    ChangeCameraState(on);
}

void G_Player::ChangeMoveState(bool on)
{
    if (on)
    {
        if (on == ec_W.IsConnected()) return;
        ec_W.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_W, { &G_Player::Callback_W,this }));
        ec_S.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_S, { &G_Player::Callback_S,this }));
        ec_A.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_A, { &G_Player::Callback_A,this }));
        ec_D.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_D, { &G_Player::Callback_D,this }));
        ec_space.NewConnection(ZC_Events::ConnectButtonClick(ZC_ButtonID::K_SPACE, { &G_Player::Callback_Space,this }, nullptr));
    }
    else
    {
        if (on == ec_W.IsConnected()) return;
        ec_W.Disconnect();
        ec_S.Disconnect();
        ec_A.Disconnect();
        ec_D.Disconnect();
        ec_space.Disconnect();
    }
}

void G_Player::ChangeCameraState(bool on)
{
    camera.SetConnectionToEvents(on);
}

void G_Player::SetDefaultState()
{
    upObj->VSetDefaultState_IO();
    camera.SetDefaultState({ 0.f, 0.f, upObj->GetRadius() });   //  must be before after upObj, caurse G_Object::upSK (scroll system volume), if cam updated before, cam will have pos on start while player there where dead
    gui_w_health.SetDefaultState();
}

void G_Player::SetSoundSetToDefault()
{
    upObj->SetSoundSetToDefault();
}

float G_Player::GetRadius()
{
    return upObj->GetRadius();
}

float G_Player::GetHealth()
{
    return upObj->GetHealth();
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

    ZC_Vec3<float> cam_dir_front = ZC_Vec::Normalize(upObj->VGetPosition_IO() - camera.cam.GetPosition());
    dir_right = ZC_Vec::Normalize(ZC_Vec::Cross(cam_dir_front, world_up));
    dir_front = ZC_Vec::Normalize(ZC_Vec::Cross(world_up, dir_right));

    dirs_actual = true;
}

void G_Player::CallbackPlayerInfo(G_PlayerInfro player_info)
{
    switch (player_info)
    {
    case G_PI__position: camera.SetCameraLookOn(upObj->VGetPosition_IO()); break;
    case G_PI__health:
    {
        if (ec_updater.IsConnected()) return;
        float health = upObj->GetHealth();
        if (health <= 0.f)
        {
            gui_w_health.UpdateHealth(0.f, upObj->GetTypeOfLastDamager());
            ChangeMoveState(false);
            ec_updater.NewConnection(ZC__Updater::Connect({ &G_Player::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play));
        }
        else gui_w_health.UpdateHealth(health, upObj->GetTypeOfLastDamager());
    } break;
    case G_PI__cam_rotate_angle_z: camera.RotateCameraHorizontal(upObj->GetRotateAngle()); break;
    case G_PI__fall: if (!ec_updater.IsConnected()) G_GameManager::pGM->PlayerDead(); break;
    }
}

void G_Player::Callback_Updater(float time)
{
    static const float seconds_after_death_wait = 4.f;
    static float cur_time = 0.f;

    cur_time += time;
    if (cur_time >= seconds_after_death_wait)
    {
        cur_time = 0.f;
        ec_updater.Disconnect();
        G_GameManager::pGM->PlayerDead();
    }
}