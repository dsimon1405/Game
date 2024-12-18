#pragma once

#include "G_ObjPlayable.h"

#include <list>

struct G_OP_MarbleSphere : public G_ObjPlayable
{
    enum SpacePosition
    {
        SP_Ground,
        SP_Jump,
        SP_Flight,
    };

    struct ChangableData
    {
        ZC_Vec3<float> cur_move_direction;

        ZC_Mat4<float> local_model { 1.f };

        float cur_move_speed = 0.f;

        float cur_jump_angle = 0.f;

        std::list<G_PushSet> push_sets;
        float push_angle = 0.f;

        SpacePosition space_position = SP_Ground;
        SpacePosition space_position_prev = SP_Ground;

        std::vector<ZC_Vec3<float>> move_dirs;  //  dirs added with buttons in one frame, mux 2

        bool on_ground_collision_in_prev_frame = true;

        float dmg_time = 0.f;
        ZC_Vec3<float> dmg_color;   //  rgb from wich to move to {0,0,0} color

        bool pushed_from_platform = false;
    } ch_d;

    static inline const float max_move_speed = 10.f;
    static inline const float speed_change_val = 10.f;

    static inline const float max_health = 100.f;


    G_OP_MarbleSphere(bool is_player);

    std::vector<G_GameSound> GetSounds();

    void VMoveInDirection_OP(const ZC_Vec3<float>& dir) override;
    void VMakeJump_OP() override;
    void VPushObjectInDirection_IO(const G_PushSet& move_set) override;
    void VSetDefaultState_OP() override;
    G_ObjectTypeMask VGetTypeMask_IO() const override;
    void VOnGroundRotateZ_IO(const ZC_Vec3<float>& origin, float angle) override;
    void VDamageObject_OP(float damage, G_ObjectType ot_damager) override;

    void Callback_Updater(float time);

    void PushMove(ZC_Vec3<float>& rDir, float& rSpeed, float& rPower);
    void InertionMove(float time, ZC_Vec3<float>& pos);
    void Move(float time, ZC_Vec3<float>& pos);
    void JumpMove(float time, ZC_Vec3<float>& pos);

    void Callback_Collision(const ZC_CO_CollisionResult& coll_result);

    void UpdateMatModel(const ZC_Vec3<float>& pos);

        //  normalization with check, too much fails
    bool Normalize(ZC_Vec3<float>& vec, const ZC_Vec3<float>& vec_default);
    
    void UpdateColorDMG(float time);
    void UpdateSound();




    // static inline const float max_rotation_speed = 500.f;
    // static inline const float move_rotation_speed_coef = max_rotation_speed / max_move_speed;
    // static inline const float radius = 1.f;

            //  were at struct ChangableData
        // ZC_Vec3<float> cur_rotate_direction { 0.f, 1.f, 0.f};
        // float rotate_angle_X = 0.f;
        // float rotation_signe_X = -1.f; //  store rotation signe around X axis
        // float sin_Z = 0.f;
        // float cos_Z = 1.f;  //  1 to start drawn on start (not after move), otherwise model matrix rotate aria equal 0
        // float cur_rotattion_speed = 0.f;

    // void SetMoveAndRotateSpeeds(float new_speed, float time);
    // void CalculateRotateZ(float time);
    // void CalculateCosSin();
    // void RotateZ_WithCosSin(ZC_Mat4<float>& model);
};

