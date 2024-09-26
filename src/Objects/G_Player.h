#pragma once

#include "G_Object.h"
#include "G_Camera.h"
#include <ZC/Events/ZC_EC.h>

#include <list>

struct G_PushSet
{
    ZC_Vec3<float> dir;
    float power;

    G_PushSet(const ZC_Vec3<float>& _dir, float _power);
};

struct G_Player : public G_Object
{
    enum SpacePosition
    {
        SP_Ground,
        SP_Jump,
        SP_Flight,
    };

    ZC_Vec3<float> position;
    G_Camera camera;

    ZC_EC ec_W;
    ZC_EC ec_S;
    ZC_EC ec_A;
    ZC_EC ec_D;
    ZC_EC ec_space;
    ZC_EC ec_update;


    ZC_Vec3<float> dir_front;
    ZC_Vec3<float> dir_right;
    ZC_Vec3<float> cur_move_direction;
    ZC_Vec3<float> cur_rotate_direction;

    float rotate_angle_X = 0.f;
    float rotation_signe_X = -1.f; //  store rotation signe around X axis
    float sin_Z = 0.f;
    float cos_Z = 1.f;  //  1 to start drawn on start (not after move), otherwise model matrix rotate aria equal 0

    const float max_move_speed = 10.f;
    const float speed_change_val = 10.f;
    float cur_move_speed = 0.f;

    const float max_rotation_speed = 500.f;
    const float move_rotation_speed_coef = max_rotation_speed / max_move_speed;
    float cur_rotattion_speed = 0.f;

    std::list<G_PushSet> push_sets;
    float push_angle = 0.f;

    SpacePosition space_position;

    std::vector<ZC_Vec3<float>> move_dirs;  //  dirs added with buttons in one frame, mux 2

    G_Player();

    G_Object_Type GetType() const override;

        //  move object in some direction in current frame
    void PushObjectInDirection(const G_PushSet& move_set);

    void SetEventAndUpdateState(bool use) override;

    void Callback_CameraRotated(const ZC_Vec3<float>& cam_pos);
    void Callback_ChangePosition(float time);

    void PushMove(ZC_Vec3<float>& rDir, float& rSpeed);
    void InertionMove(float time);
    void Move(float time);
    void JumpMove(float time);

    void SetMoveAndRotateSpeeds(float new_speed, float time);
    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);

    void Callback_W(ZC_ButtonID, float time);
    void Callback_S(ZC_ButtonID, float time);
    void Callback_A(ZC_ButtonID, float time);
    void Callback_D(ZC_ButtonID, float time);
    void MoveButtonPressed(const ZC_Vec3<float>& direction);
    void Callback_Space(ZC_ButtonID, float time);

    void UpdateMatModel();

    void CalculateRotateZ(float time);
    void RotateZ_WithCosSin(ZC_Mat4<float>& model);

        //  normalization with check, too much fails
    bool Normalize(ZC_Vec3<float>& vec, const ZC_Vec3<float>& vec_default);
};