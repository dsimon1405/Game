#include "G_Player.h"

#include <ZC/Events/ZC_Events.h>
#include <ZC/Tools/Math/ZC_Math.h>
#include <Model/G_Models.h>
#include <ZC/Video/ZC_SWindow.h>
#include <G_GameManager.h>

G_Player::G_Player()
    : G_Object(G_ModelName::G_MN__Sphere, 0, new ZC_CollisionObject(G_Models::GetModel_COFigure(G_MN__Sphere), ZC_C0_Type::ZC_COT__DynamicPushback, this, { &G_Player::Callback_Collision, this })),
    position(0.f, 0.f, 1.f),  //  move sphere to {0,0,0} position of the bottom on radius 1.f
    camera({ &G_Player::Callback_CameraRotated, this })
{
    move_dirs.reserve(2);   //  max pressed button for dir change in one frame 2

    UpdateMatModel();   //  update position seted in param
    Callback_CameraRotated(camera.cam.GetPosition());   //  calculate diraction on start
}

G_Object_Type G_Player::GetType() const
{
    return G_O__Sphere;
}

void G_Player::PushObjectInDirection(const G_PushSet& move_set)
{
    push_sets.emplace_back(move_set);
}

void G_Player::SetEventAndUpdateState(bool use)
{
    if (use)
    {
        if (use == ec_W.IsConnected()) return;
        ec_W.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_W, { &G_Player::Callback_W,this }));
        ec_S.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_S, { &G_Player::Callback_S,this }));
        ec_A.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_A, { &G_Player::Callback_A,this }));
        ec_D.NewConnection(ZC_Events::ConnectButtonPressDown(ZC_ButtonID::K_D, { &G_Player::Callback_D,this }));
        ec_space.NewConnection(ZC_Events::ConnectButtonClick(ZC_ButtonID::K_SPACE, { &G_Player::Callback_Space,this }, nullptr));
        ec_update.NewConnection(ZC_SWindow::ConnectUpdate({ &G_Player::Callback_ChangePosition, this }, G_UL__update));
        camera.SetConnectionToEvents(use);
    }
    else
    {
        if (use != ec_W.IsConnected()) return;
        ec_W.Disconnect();
        ec_S.Disconnect();
        ec_A.Disconnect();
        ec_D.Disconnect();
        ec_space.Disconnect();
        ec_update.Disconnect();
        camera.SetConnectionToEvents(use);
    }
}

void G_Player::Callback_CameraRotated(const ZC_Vec3<float>& cam_pos)
{
    static const ZC_Vec3<float> world_up(0.f,0.f,1.f);

    ZC_Vec3<float> cam_dir_front = ZC_Vec::Normalize(position - cam_pos);
    dir_right = ZC_Vec::Normalize(ZC_Vec::Cross(cam_dir_front, world_up));
    dir_front = ZC_Vec::Normalize(ZC_Vec::Cross(world_up, dir_right));
}

void G_Player::Callback_ChangePosition(float time)
{
    if (position[2] < -8.f) position = ZC_Vec3<float>(0.f, 0.f, 10.f);

    // PushObjectInDirection(G_PushSet({ 1, 0, 0 }, 0.8f ));




    if (space_position == SP_Jump) JumpMove(time);
    else if (move_dirs.empty()) InertionMove(time);    //  no one effects the body, make inertion
    else Move(time);

        //  gravitation
    if (space_position != SP_Jump)
    {
        const float gravitation_power = 10.f;
        position[2] -= gravitation_power * time;
    }

    CalculateRotateZ(time);
    UpdateMatModel();
}

void G_Player::SetMoveAndRotateSpeeds(float new_speed, float time)
{
    cur_move_speed = new_speed;
    
    cur_rotattion_speed = cur_move_speed * move_rotation_speed_coef * time * rotation_signe_X;
    float resutl = rotate_angle_X + cur_rotattion_speed;
    rotate_angle_X = resutl >= 360.f ? resutl - 360.f
                    : resutl < 0.f ? 360.f - resutl
                    : resutl;
}

void G_Player::PushMove(ZC_Vec3<float>& rDir, float& rSpeed)
{
    if ((push_sets.empty())) return;

    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    for (G_PushSet& ms : push_sets)
    {
        if (!Normalize(ms.dir, {})) continue;
        push_speed += speed_change_val * (cur_move_direction != ZC_Vec3<float>() ? ZC_Vec::Dot(cur_move_direction, ms.dir) : 1.f) * ms.power;
        push_dir += ms.dir * ms.power;
    }
    push_dir /= push_sets.size();
    push_speed /= push_sets.size();
    push_sets.clear();

    rDir = push_dir;
    rSpeed = push_speed;
}

void G_Player::InertionMove(float time)
{
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    PushMove(push_dir, push_speed);
    if (push_speed != 0.f)
    {       // dir
        cur_move_direction = (cur_move_direction + push_dir) / 2.f;
        Normalize(cur_move_direction, {});
            //  speed
        push_speed *= time;  //  time control to changing current move speed
        cur_move_speed += push_speed;  //  time control to changing current move speed
        if (cur_move_speed > max_move_speed) cur_move_speed = max_move_speed;
        SetMoveAndRotateSpeeds(cur_move_speed, time);  //  time control to changing current move speed
        position += cur_move_direction * push_speed;     //  time control to move lenth
        assert(position[0] * 0.f == 0.f || position[1] * 0.f == 0.f || position[2] * 0.f == 0.f);
    }
    else if (cur_move_speed != 0.f)  //  object are stoped
    {
        float new_move_speed = cur_move_speed - (speed_change_val * time);  //  time control to changing current move speed
        if (cur_move_speed <= 0.f)
        {
            SetMoveAndRotateSpeeds(0.f, time);
            cur_move_direction = {};
        }
        else
        {
            SetMoveAndRotateSpeeds(new_move_speed, time);
            position += cur_move_direction * cur_move_speed * time;     //  time control to move lenth
            assert(position[0] * 0.f == 0.f || position[1] * 0.f == 0.f || position[2] * 0.f == 0.f);
        }
    }
}

void G_Player::Move(float time)
{
    ZC_Vec3<float> new_dir;
    float add_speed = 0.f;
    for (ZC_Vec3<float>& md : move_dirs)
    {
        new_dir += md;
        add_speed += speed_change_val * (cur_move_direction != ZC_Vec3<float>() ? ZC_Vec::Dot(cur_move_direction, md) : 1.f);
    }
    new_dir /= move_dirs.size();
    add_speed /= move_dirs.size();
    move_dirs.clear();
        //  push add
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    PushMove(push_dir, push_speed);
    if (push_speed != 0.f)
    {
        new_dir = (new_dir + push_dir) / 2.f;
        add_speed = (add_speed + push_speed) / 2.f;
    }
        //  calc dir
    if (new_dir != ZC_Vec3<float>())
    {
        add_speed *= time;  //  time control to changing current move speed
        float new_speed = cur_move_speed + add_speed;
        float cos = ZC_Vec::Dot(cur_move_direction, new_dir);
        if (cos < -0.3f)
        {
            if (new_speed < 0.f)
            {
                new_speed *= -1.f;
                    //  dir
                cur_move_direction = (cur_move_direction + new_dir) / 2.f;
                Normalize(cur_move_direction, {});
            }
            SetMoveAndRotateSpeeds(new_speed, time);
        }
        else
        {       //  dir
            cur_move_direction = (cur_move_direction + new_dir) / 2.f;
            Normalize(cur_move_direction, {});
            SetMoveAndRotateSpeeds(new_speed > max_move_speed ? max_move_speed : new_speed, time);
        }

        position += cur_move_direction * cur_move_speed * time;     //  time control to move lenth
        assert(position[0] * 0.f == 0.f || position[1] * 0.f == 0.f || position[2] * 0.f == 0.f);
    }
}

void G_Player::JumpMove(float time)
{
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    PushMove(push_dir, push_speed);
    if (push_speed != 0.f)
    {       // dir
        cur_move_direction = (cur_move_direction + push_dir) / 2.f;
        Normalize(cur_move_direction, {});
            //  speed
        const float flight_push_coef = 2.f;
        push_speed *= flight_push_coef * time;  //  time control to changing current move speed
        cur_move_speed += push_speed;  //  time control to changing current move speed
        if (cur_move_speed > max_move_speed) cur_move_speed = max_move_speed;
    }

    const float start_jump_angle = 90.f;    //  for cos
    const float jump_angle_power = 220.f;
    float jump_angle_coef = 2.f - cur_move_speed / max_move_speed;  //  want to have with lower speed short fast jump
    static float cur_jump_angle = 0.f;
    cur_jump_angle += jump_angle_power * jump_angle_coef * time;   //  jump angle time controll 
    
    if (cur_jump_angle >= 335.f || cur_move_speed < 1.f)    //  finish jump
    {
        space_position = SP_Flight;
        cur_jump_angle = 0.f;
        return;
    }

    float flight_speed = cur_move_speed * 2.f;
    float cos_flight_angle = std::cos(ZC_Vec::Radians(start_jump_angle - cur_jump_angle));

    position[0] += cur_move_direction[0] * (push_speed == 0.f ? cur_move_speed * time : push_speed);
    position[1] += cur_move_direction[1] * (push_speed == 0.f ? cur_move_speed * time : push_speed);
    position[2] += cos_flight_angle * flight_speed * time;

    assert(position[0] * 0.f == 0.f || position[1] * 0.f == 0.f || position[2] * 0.f == 0.f);
    
    SetMoveAndRotateSpeeds(cur_move_speed, time);
}

void G_Player::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    position += coll_result.pushback;
    space_position = SP_Ground;  //  may be change in next cycle
        //  check aphere out of platform
    if (static_cast<G_Object*>(coll_result.pObj->GetHolder())->GetType() == G_O__PlatformQuad)
    {       //  check by radious
        const ZC_CO_Figure& pObj_fig = coll_result.pObj->GetFigure();
        ZC_Vec3<float> dist = pObj_fig.center_fact - position;
        if (ZC_Vec::Length<float>({ dist[0], dist[1], 0.f }) > 0.7f)    //  sphere's center may be out of one of the platform edges
        {       //  check platform side surfaces normals on out of
            for (const ZC_CO_Surface<ZC_Vec3<float>*>& surf : pObj_fig.surfaces_fact)
            {       //  side normal (not top or bottom)
                if ((*surf.normal)[2] == 0.f && ZC_DistanceFromPointToPlane(*(surf.points.front()), *(surf.normal), position) > 0.f)
                {
                    space_position = SP_Flight;     //  spheres center still inside of the platdorm
                    position = ZC_Vec::MoveByLength(position, *(surf.normal), 0.1f);    //  sphere out of platform, but don't want to fall, lets help them
                }
            }
        }
    }

    UpdateMatModel();
}

void G_Player::Callback_W(ZC_ButtonID, float time)
{
    MoveButtonPressed(dir_front);
}

void G_Player::Callback_S(ZC_ButtonID, float time)
{
    MoveButtonPressed(dir_front * -1.f);
}

void G_Player::Callback_A(ZC_ButtonID, float time)
{
    MoveButtonPressed(dir_right * -1.f);
}

void G_Player::Callback_D(ZC_ButtonID, float time)
{
    MoveButtonPressed(dir_right);
}

void G_Player::MoveButtonPressed(const ZC_Vec3<float>& direction)
{
    if (space_position != SP_Ground || move_dirs.size() == 2) return;
    move_dirs.emplace_back(direction);
}

void G_Player::Callback_Space(ZC_ButtonID, float time)
{
    if (space_position != SP_Ground) return;
    move_dirs.clear();
    space_position = SP_Jump;
}

void G_Player::UpdateMatModel()
{
    ZC_Mat4<float> model(1.f);
    model.Translate(position);
    assert(model[3][0] * 0.f == 0.f || model[3][1] * 0.f == 0.f || model[3][2] * 0.f == 0.f || model[3][3] * 0.f == 0.f);
    RotateZ_WithCosSin(model);
    model.Rotate(rotate_angle_X, { 1.f, 0.f, 0.f });
    this->upCO->UpdateModelMatrix(model);

    camera.SetCameraLookOn(position);
}

void G_Player::CalculateRotateZ(float time)
{
        //  rotate speres origin to correct (new) direction
    const float rotate_power = 10.f;

    if (cur_move_speed == 0.f) return;

    float cos_dir = ZC_Vec::Dot(cur_move_direction, cur_rotate_direction);
    if (cos_dir < -0.8f)    //  dirs allmost oposite, help to rotate
    {       //  avoid arthogonal rotation for X change movind direction. Object will change rotation_signe_X downer and start reverce rotate around X 
        if ((cur_rotate_direction[1] > 0.f && cur_rotate_direction[1] >= 0.8f) || (cur_rotate_direction[1] < 0.f && cur_rotate_direction[1] <= -0.8f))
            cur_rotate_direction = {};
        else    //  for Y axis changing orthogonal diraction make some help, rotate current rotate diraction on 1 degree (wrong way, but help)
        {
            ZC_Vec4<float> cur_rot_dir_v4 = ZC_Mat4<float>(1.f).Rotate(1.f, { 0.f, 0.f, -1.f }) * ZC_Vec4<float>(cur_rotate_direction, 1.f);
            cur_rotate_direction = ZC_Vec3<float>(cur_rot_dir_v4[0], cur_rot_dir_v4[1], cur_rot_dir_v4[2]);
        }
    }

    ZC_Vec3<float> cur_to_new_offset = cur_move_direction - cur_rotate_direction;   //  where to rotate from current rotation angle
    ZC_Vec3<float> temp_rotate_diration = cur_rotate_direction + cur_to_new_offset * rotate_power * time;   //  calculate new rotate diraction with time controll
    if (temp_rotate_diration != ZC_Vec3<float>())
    {          //  check normalizatin
        Normalize(temp_rotate_diration, cur_rotate_direction);
        cur_rotate_direction = temp_rotate_diration;
    }
    else return;    //  no rotation, out


        //  calculate cos sin forrotation around Z axis
    if (cur_move_direction == ZC_Vec3<float>()) return;
        //  take positive X for calculateion.
    sin_Z = ZC_Vec::Dot({ 1.f, 0.f, 0.f }, cur_rotate_direction);  //  usual Dot return cos, but now it sin, don't know why
    cos_Z = std::sqrt(1.f - (sin_Z * sin_Z));

    assert(cos_Z * 0.f == 0.f);
        //  correct cos sin signes with axises of current roatate vector
    if ((cur_rotate_direction[0] >= 0.f && cur_rotate_direction[1] >= 0.f) || (cur_rotate_direction[0] <= 0.f && cur_rotate_direction[1] >= 0.f))
    {
        sin_Z *= -1.f;
        rotation_signe_X = -1.f;    //  x diractin rotation changed
    }
    else if ((cur_rotate_direction[0] <= 0.f && cur_rotate_direction[1] <= 0.f) || (cur_rotate_direction[0] >= 0.f && cur_rotate_direction[1] <= 0.f))
    {
        sin_Z *= 1.f;
        cos_Z *= 1.f;
        rotation_signe_X = 1.f;    //  x diractin rotation changed
    }
    
    // float radians = ZC_Vec::Radians(rotate_angle_Z + temp_rotate_angle_Z);
    // float sin1 = std::sin(radians);
    // float cos1 = std::cos(radians);
}

void G_Player::RotateZ_WithCosSin(ZC_Mat4<float>& model)
{
        //  Code taken from ZC_Mat4::Roate
        //  Rotate model matri by Z axis using cos and sin calculated in G_Player::CalculateRotateZ()
        //  Object start pos must be at {0,0,0} and look in {0,1,0} direction
    ZC_Vec3<float> axis(0.f, 0.f, 1.f);
    ZC_Vec3<float> temp(axis * (static_cast<float>(1) - cos_Z));

    ZC_Vec3<float> rotateX;
    rotateX[0] = cos_Z + temp[0] * axis[0];
    rotateX[1] = temp[0] * axis[1] + sin_Z * axis[2];
    rotateX[2] = temp[0] * axis[2] - sin_Z * axis[1];
    ZC_Vec4<float> columnX = model[0] * rotateX[0] + model[1] * rotateX[1] + model[2] * rotateX[2];

    ZC_Vec3<float> rotateY;
    rotateY[0] = temp[1] * axis[0] - sin_Z * axis[2];
    rotateY[1] = cos_Z + temp[1] * axis[1];
    rotateY[2] = temp[1] * axis[2] + sin_Z * axis[0];
    ZC_Vec4<float> columnY = model[0] * rotateY[0] + model[1] * rotateY[1] + model[2] * rotateY[2];

    ZC_Vec3<float> rotateZ;
    rotateZ[0] = temp[2] * axis[0] + sin_Z * axis[1];
    rotateZ[1] = temp[2] * axis[1] - sin_Z * axis[0];
    rotateZ[2] = cos_Z + temp[2] * axis[2];
    ZC_Vec4<float> columnZ = model[0] * rotateZ[0] + model[1] * rotateZ[1] + model[2] * rotateZ[2];

    model[0] = columnX;
    model[1] = columnY;
    model[2] = columnZ;
}

bool G_Player::Normalize(ZC_Vec3<float>& vec, const ZC_Vec3<float>& vec_default)
{
    assert(vec[0] * 0.f == 0.f || vec[1] * 0.f == 0.f || vec[2] * 0.f == 0.f);
    if (vec != ZC_Vec3<float>()) vec = ZC_Vec::Normalize(vec);
    assert(vec[0] * 0.f == 0.f || vec[1] * 0.f == 0.f || vec[2] * 0.f == 0.f);
    if (vec[0] * 0.f != 0.f || vec[1] * 0.f != 0.f || vec[2] * 0.f != 0.f)
    {
        vec = vec_default;    //  dangerous place, avoid none in Normalize!
        return false;
    }
    return true;
}





    //  G_PushSet

G_PushSet::G_PushSet(const ZC_Vec3<float>& _dir, float _power)
    : dir(_dir == ZC_Vec3<float>() ? _dir : ZC_Vec::Normalize(_dir)),
    power(_power)
{}


















    // enum MoveDirection
    // {
    //     None    = 0,
    //     Front   = 1,
    //     Back    = 1 << 1,
    //     Left    = 1 << 2,
    //     Right   = 1 << 3,
    //     Push    = 1 << 4
    // };

    // int cur_dir_mask = None;     //  mask MoveDirection
    // int prev_dir_mask = None;     //  mask MoveDirection
    // int inertion_dir_mask = None;







// void G_Player::UpdateRotateAxis(int dir_mask, float time)
// {
//     float temp_angle_Z = 0.f;
//     switch (dir_mask)
//     {
//     case Front:
//     {
//         rotation_signe_X = -1.f;
//     } break;
//     case Back:
//     {
//         rotation_signe_X = 1.f;
//     } break;
//     case Left:
//     {
//         rotation_signe_X = -1.f;
//         temp_angle_Z = 90.f;
//     } break;
//     case Right:
//     {
//         rotation_signe_X = -1.f;
//         temp_angle_Z = -90.f;
//     } break;
//     case Front | Left:
//     {
//         rotation_signe_X = -1.f;
//         temp_angle_Z = 45.f;
//     } break;
//     case Front | Right:
//     {
//         rotation_signe_X = -1.f;
//         temp_angle_Z = -45.f;
//     } break;
//     case Back | Left:
//     {
//         rotation_signe_X = 1.f;
//         temp_angle_Z = -45.f;
//     } break;
//     case Back | Right:
//     {
//         rotation_signe_X = 1.f;
//         temp_angle_Z = 45.f;
//     } break;
//     case Push:
//     {
//         rotation_signe_X = 1.f;
//         temp_angle_Z = push_angle;
//     }break;
//     default: break;
//     }

//     if (temp_angle_Z == temp_rotate_angle_Z) return;
//     const float rotate_power = 4.f;
//     if ((temp_angle_Z <= 0.f && temp_rotate_angle_Z <= 0.f) || (temp_angle_Z >= 0.f && temp_rotate_angle_Z >= 0.f))
//     {
//         float offset_angle = temp_angle_Z - temp_rotate_angle_Z;
//         temp_rotate_angle_Z += offset_angle * rotate_power * time;
//     }
//     else if ((temp_angle_Z >= 0.f && temp_rotate_angle_Z <= 0.f) || (temp_angle_Z <= 0.f && temp_rotate_angle_Z >= 0.f))
//     {
//         float offset_angle = std::abs(temp_angle_Z) + std::abs(temp_rotate_angle_Z);
//         temp_rotate_angle_Z += offset_angle * rotate_power * time * (temp_angle_Z >= 0 ? 1.f : -1.f);
//     }
// }





// void G_Player::UpdateModelWithLeftOrRightRotation(float time, bool move_to_left, bool update_pos)
// {
    // rotate_angle_X += rotattion_speed * time;
    
    // auto lamb_Set = [this, move_to_left, update_pos, time](float rot_angle, bool front_or_back_move)
    // {
    //     rotate_angle_Z += rot_angle;
    //     if (update_pos)
    //     {
    //         ZC_Vec3<float> offset = move_to_left ? dir_right * -1 * time * move_speed : dir_right * time * move_speed;
    //         if (front_or_back_move) offset /= two_dirs_divisor;      //  need add only half of offset, first half was added in front/back move callback
    //         position += offset;
    //     }
    //     return rot_angle;
    // };

    // float rot_angle = move_to_left ? (ZC_Events::IsButtonPressed(ZC_ButtonID::K_W) ? lamb_Set(-135.f, true) : ZC_Events::IsButtonPressed(ZC_ButtonID::K_S) ? lamb_Set(-45.f, true) : lamb_Set(-90.f, false))
    //                                 : (ZC_Events::IsButtonPressed(ZC_ButtonID::K_W) ? lamb_Set(135.f, true) : ZC_Events::IsButtonPressed(ZC_ButtonID::K_S) ? lamb_Set(45.f, true) : lamb_Set(90.f, false));
                        
    // UpdateMatModel();
    // rotate_angle_Z -= rot_angle;
// }

// void G_Player::MakeFrontOrBackMove(const ZC_Vec3<float>& offset, float rotate)
// {
//     position += offset;
//     float resutl = rotate_angle_X + rotate;
//     rotate_angle_X = resutl >= 360.f ? resutl - 360.f
//                     : resutl < 0.f ? 360.f - resutl
//                     : resutl;
//     UpdateMatModel();
// }



        // ZC_Vec3<float> new_dir;
        // float add_speed = 0.f;
        // for (G_PushSet& ms : move_sets)
        // {
        //     new_dir += ms.dir * ms.power;
        //     float cos = ZC_Vec::Dot(move_dir, ms.dir);
        //     add_speed += speed_change_val * cos * ms.power;
        // }
        // new_dir /= move_sets.size();
        // move_sets.clear();
        // if (new_dir != ZC_Vec3<float>())
        // {

        //         //  dir
        //     move_dir = (move_dir + new_dir) / 2.f;
        //     if (move_dir != ZC_Vec3<float>()) move_dir = ZC_Vec::Normalize(move_dir);

        //     float new_speed = cur_move_speed + add_speed;
        //     if (new_speed < 0) new_speed = 0.f;
        //     SetMoveAndRotateSpeeds(new_speed > max_move_speed ? max_move_speed : new_speed);



        //     position += move_dir * cur_move_speed * time;
        // }



            //  MOST WORKING
        // float force = 1.f;

        // ZC_Vec3<float> offset_dir;
        // ZC_Vec3<float> new_dir;
        // for (MoveSet& ms : move_sets) offset_dir += ms.dir;
        // offset_dir /= move_sets.size();
        // if (offset_dir == ZC_Vec3<float>()) return;
        // move_sets.clear();
        // move_dir = (move_dir + (offset_dir * force)) / 2.f;
        // if (move_dir != ZC_Vec3<float>()) move_dir = ZC_Vec::Normalize(move_dir);

        
        // float cos = ZC_Vec::Dot(move_dir, ZC_Vec::Normalize(offset_dir));
        // float add_speed = std::abs(speed_change_val * (cos * 1));
        // float new_speed = cur_move_speed + add_speed;    
        // cur_move_speed = new_speed > max_move_speed ? max_move_speed : new_speed;

        // position += move_dir * cur_move_speed * time;






        // ZC_Vec3<float> move_dir_normalized;
        // if (move_dir != ZC_Vec3<float>()) move_dir_normalized = ZC_Vec::Normalize(move_dir);

        // ZC_Vec3<float> co_directional_dirs;
        // float co_directional_dirs_count = 0.f;

        // ZC_Vec3<float> conter_directional_dirs;
        // float conter_directional_dirs_count = 0.f;

        // ZC_Vec3<float> offset_dir;
        // float offset_speed = 0.f;
        // for (MoveSet& ms : move_sets)
        // {
        //     if (ms.dir[1] == -1.f)
        //     {
        //         int q = 2;
        //     }
        //     // offset_dir += ms.dir * ms.power;

        //     float cos = ZC_Vec::Dot(ms.dir, move_dir_normalized);
        //     // float cos = ZC_Vec::Dot(ms.dir, move_dir);

        //     if (cos >= 0)
        //     {
        //         co_directional_dirs += ms.dir * ms.power;
        //         ++co_directional_dirs_count;
        //     }
        //     else
        //     {
        //         conter_directional_dirs += ms.dir * ms.power;
        //         ++conter_directional_dirs_count;
        //     }

        //     offset_speed += speed_change_val * cos * ms.power;
        // }
        // // offset_dir /= move_sets.size();
        // // if (offset_dir != ZC_Vec3<float>()) offset_dir = ZC_Vec::Normalize(offset_dir);
        // offset_speed /= move_sets.size();
        // move_sets.clear();
        //     //  dir
        // float new_speed = cur_move_speed + offset_speed;
        // if (new_speed > 0)  //  front direction changed not more then on 90 degree 
        // {
        //     if (co_directional_dirs_count > 0.f) offset_dir = co_directional_dirs / co_directional_dirs_count;
        //     cur_move_speed = new_speed;
        // }
        // else if (new_speed < 0)     //  direction changed mothen on 90 degree
        // {
        //     if (conter_directional_dirs_count > 0.f) offset_dir = conter_directional_dirs / conter_directional_dirs_count;
        //     cur_move_speed = new_speed * -1.f;
        // }
        // else    //  obect stoped
        // {
        //     offset_dir = (co_directional_dirs + conter_directional_dirs) / (co_directional_dirs_count + conter_directional_dirs_count);
        // }

        // // if (offset_dir != ZC_Vec3<float>()) offset_dir = ZC_Vec::Normalize(offset_dir);
        // move_dir = (move_dir + offset_dir) / 2.f;

        // // if (move_dir != ZC_Vec3<float>()) move_dir = ZC_Vec::Normalize(move_dir);
        // // if (move_dir == ZC_Vec3<float>()) return;   //  object stoped
        // // else move_dir = ZC_Vec::Normalize(move_dir);
        
        // //     //  speed
        // // cur_move_speed += offset_speed;
        // // if (cur_move_speed == 0) return;    //  object stoped
        // if (cur_move_speed > max_move_speed) cur_move_speed = max_move_speed;

        // if (move_dir != ZC_Vec3<float>()) move_dir_normalized = ZC_Vec::Normalize(move_dir);
        // // position += move_dir * cur_move_speed * time;
        // position += move_dir_normalized * cur_move_speed * time;