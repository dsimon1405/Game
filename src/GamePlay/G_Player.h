#pragma once

#include <ZC/Tools/ZC_uptr.h>
#include "G_Camera.h"
#include <ZC/Events/ZC_EC.h>
#include <Objects/G_ObjPlayable.h>
#include "GUI/G_GUI_P_W_Health.h"

class G_Player
{
public:
    G_Player();
    ~G_Player();
    
    void ChangeActivityState(bool on);
    void ChangeMoveState(bool on);
    void ChangeCameraState(bool on);
    void SetDefaultState();
    void SetSoundSetToDefault();
    float GetRadius();

private:
    G_Camera camera;
    ZC_uptr<G_ObjPlayable> upObj;

    ZC_EC ec_W;
    ZC_EC ec_S;
    ZC_EC ec_A;
    ZC_EC ec_D;
    ZC_EC ec_space;
    ZC_EC ec_updater;

    ZC_Vec3<float> dir_front;
    ZC_Vec3<float> dir_right;

    bool dirs_actual = false;

    G_GUI_P_W_Health gui_w_health;

    void Callback_CameraRotated(const ZC_Vec3<float>& cam_pos);
    void Callback_W(ZC_ButtonID, float time);
    void Callback_S(ZC_ButtonID, float time);
    void Callback_A(ZC_ButtonID, float time);
    void Callback_D(ZC_ButtonID, float time);
    void Callback_Space(ZC_ButtonID, float time);
    void RecalculateDirection();
    void CallbackPlayerInfo(G_PlayerInfro player_info);
    void Callback_Updater(float time);
};