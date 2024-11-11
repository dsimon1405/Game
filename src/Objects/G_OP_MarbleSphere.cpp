#include "G_OP_MarbleSphere.h"

#include <ZC/ZC__System.h>
#include <Model/G_Models.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_Func.h>
#include <Sound/G_SoundName.h>
#include <ZC/Tools/ZC_Random.h>

G_OP_MarbleSphere::G_OP_MarbleSphere(bool is_player)
    : G_ObjPlayable(G_ModelName::G_MN__SphereMarble, 0, new ZC_CollisionObject(G_Models::GetModel_COFigure(G_MN__SphereMarble), ZC_C0_Type::ZC_COT__DynamicPushback,
        this, { &G_OP_MarbleSphere::Callback_Collision, this }), max_health, ZC_uptr<G_GameSoundSet>(new G_GameSoundSet(GetSounds(), is_player)))
{
    ch_d.move_dirs.reserve(2);   //  max pressed button for dir change in one frame 2

    this->ecUpdater = ZC__Updater::Connect({ &G_OP_MarbleSphere::Callback_Updater, this }, G_UL__game_play);
}

std::vector<G_GameSound> G_OP_MarbleSphere::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(5);
    sounds.emplace_back(G_GameSound(G_SN__sphere_move));
    sounds.emplace_back(G_GameSound(G_SN__sphere_flight));
    sounds.emplace_back(G_GameSound(G_SN__sphere_jump));
    sounds.emplace_back(G_GameSound(G_SN__sphere_lands));
    sounds.emplace_back(G_GameSound(G_SN__sphere_arson));
    return sounds;
}

G_ObjectTypeMask G_OP_MarbleSphere::VGetTypeMask_IO() const
{
    return object_type | G_OT__Damageable | G_OT__Pushable;
}

void G_OP_MarbleSphere::VOnGroundRotateZ_IO(const ZC_Vec3<float>& origin, float angle)
{
    if (ch_d.space_position != SP_Ground) return;   //  object not on the ground

    const ZC_Vec3<float> cur_pos = this->VGetPosition_IO();
    ZC_Vec3<float> pos = cur_pos - origin;   //  pos in that origin space
    ZC_Vec3<float> rotated_pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle, { 0.f, 0.f, 1.f }) * ZC_Vec4<float>(pos, 1.f));
    ZC_Vec3<float> dir_distance = rotated_pos - pos;    //  dir from prev pos to new pos in origin space
    ZC_Vec3<float> new_pos = cur_pos + dir_distance;

    if (this->callback_player_info)
    {
        this->changable_data_op.rotate_angle_z = -angle;
        this->callback_player_info(G_PI__cam_rotate_angle_z);
    }
    
    ch_d.local_model = ZC_Mat4<float>(1.f).Rotate(angle, { 0.f, 0.f, 1.f }) * ch_d.local_model;    //  roatate local_model by Z

    UpdateMatModel(new_pos);
}

void G_OP_MarbleSphere::VDamageObject_OP(float damage, G_ObjectType ot_damager)
{
    static const float max_dmg = 15.f;  //  some dmg coef (max dmg of platform)

    ch_d.dmg_time = 0.f;
    float color_channel_value = damage / max_dmg;
    if (color_channel_value > 1.f) color_channel_value = 1.f;   //  some move powerfull damager
    switch (ot_damager)
    {
    case G_OT__Star:
    {
        ch_d.dmg_color = ZC_Vec3<float>(color_channel_value, color_channel_value / 3.6f, 0.f);     //  orange color
        this->upSK->AddTempSound(G_GameSound(G_SoundName(G_SN__sphere_arson)));
    } break;
    case G_OT__Platform:
    {
        ch_d.dmg_color = ZC_Vec3<float>(color_channel_value, 0.f, 0.f);  //  red color
        this->upSK->AddTempSound(G_GameSound(G_SoundName(G_SN__sphere_dmg_1 + ZC_Random::GetRandomInt(0, 1))));
    } break;
    default: assert(false); break;
    }

}

void G_OP_MarbleSphere::VMoveInDirection_OP(const ZC_Vec3<float>& dir)
{
    if (ch_d.space_position != SP_Ground || ch_d.move_dirs.size() == 2ull) return;     //  sphere is flying or, reached max of move dirs in frame 2 (2 pressed button at the same time max)
    ch_d.move_dirs.emplace_back(dir);
}

void G_OP_MarbleSphere::VMakeJump_OP()
{
    if (ch_d.space_position != SP_Ground) return;
    ch_d.move_dirs.clear();
    ch_d.space_position = SP_Jump;
}

void G_OP_MarbleSphere::VPushObjectInDirection_IO(const G_PushSet& move_set)
{
    ch_d.push_sets.emplace_back(move_set);
}

void G_OP_MarbleSphere::VSetDefaultState_OP()
{
    ch_d = {};
    this->unColor = 0;
}

void G_OP_MarbleSphere::Callback_Updater(float time)
{
    if (this->VGetPosition_IO()[2] < -30.f)
    {
        this->callback_player_info(G_PI__fall);
        return;
    }

        //  correct space posiition
    switch (ch_d.space_position)
    {
    case SP_Ground: if (!(ch_d.on_ground_collision_in_prev_frame)) ch_d.space_position = SP_Flight; break;
    case SP_Flight: if (ch_d.on_ground_collision_in_prev_frame) ch_d.space_position = SP_Ground; break;
    case SP_Jump: break;    //  chages only in JumpMove()
    }
    ch_d.on_ground_collision_in_prev_frame = false;     //  must be after UpdateSound

    ZC_Vec3<float> pos = this->VGetPosition_IO();
        //  make moves/pushes
    if (ch_d.space_position == SP_Jump) JumpMove(time, pos);
    else if (ch_d.move_dirs.empty()) InertionMove(time, pos);    //  no one effects the body, make inertion
    else Move(time, pos);

        //  gravitation
    if (ch_d.space_position != SP_Jump)
    {
        const float gravitation_power = 10.f;
        pos[2] -= gravitation_power * time;
    }

    UpdateSound();
    UpdateColorDMG(time);
    UpdateMatModel(pos);
    
    ch_d.space_position_prev = ch_d.space_position;
}

void G_OP_MarbleSphere::PushMove(ZC_Vec3<float>& rDir, float& rSpeed, float& rPower)
{
    if ((ch_d.push_sets.empty())) return;

    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    for (G_PushSet& ms : ch_d.push_sets)
    {
        if (!Normalize(ms.dir, {})) continue;
        push_speed += speed_change_val * (ch_d.cur_move_direction != ZC_Vec3<float>() ? ZC_Vec::Dot(ch_d.cur_move_direction, ms.dir) : 1.f) * ms.power;
        push_dir += ms.dir * ms.power;
        rPower += ms.power;
    }
    push_dir /= ch_d.push_sets.size();
    push_speed /= ch_d.push_sets.size();
    rPower /= ch_d.push_sets.size();
    ch_d.push_sets.clear();

    rDir = push_dir;
    rSpeed = push_speed;
}

void G_OP_MarbleSphere::InertionMove(float time, ZC_Vec3<float>& pos)
{
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    float push_power = 0.f;
    PushMove(push_dir, push_speed, push_power);
    if (push_speed != 0.f)
    {       // dir
        ch_d.cur_move_direction = (ch_d.cur_move_direction + push_dir) / 2.f;
        Normalize(ch_d.cur_move_direction, {});
            //  speed
        push_speed *= time;  //  time control to changing current move speed
        ch_d.cur_move_speed += push_speed;  //  time control to changing current move speed
        float max_push_speed = max_move_speed * push_power;
        if (ch_d.cur_move_speed > max_push_speed) ch_d.cur_move_speed = max_push_speed;
        
        pos += ch_d.cur_move_direction * ch_d.cur_move_speed * time;     //  time control to move lenth
        assert(pos[0] * 0.f == 0.f || pos[1] * 0.f == 0.f || pos[2] * 0.f == 0.f);
    }
    else if (ch_d.cur_move_speed != 0.f)  //  object are stoped
    {
        float new_move_speed = ch_d.cur_move_speed - (speed_change_val * time);  //  time control to changing current move speed
        if (ch_d.cur_move_speed <= 0.f)
        {
            ch_d.cur_move_speed = 0.f;
            ch_d.cur_move_direction = {};
        }
        else
        {
            ch_d.cur_move_speed = new_move_speed;
            pos += ch_d.cur_move_direction * ch_d.cur_move_speed * time;     //  time control to move lenth
            assert(pos[0] * 0.f == 0.f || pos[1] * 0.f == 0.f || pos[2] * 0.f == 0.f);
        }
    }
}

void G_OP_MarbleSphere::Move(float time, ZC_Vec3<float>& pos)
{
    ZC_Vec3<float> new_dir;
    float add_speed = 0.f;
    for (ZC_Vec3<float>& md : ch_d.move_dirs)
    {
        new_dir += md;
        add_speed += speed_change_val * (ch_d.cur_move_direction != ZC_Vec3<float>() ? ZC_Vec::Dot(ch_d.cur_move_direction, md) : 1.f);
    }
    new_dir /= ch_d.move_dirs.size();
    add_speed /= ch_d.move_dirs.size();
    ch_d.move_dirs.clear();
        //  push add
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    float push_power = 0.f;     //  unused yet
    PushMove(push_dir, push_speed, push_power);
    if (push_speed != 0.f)
    {
        new_dir = (new_dir + push_dir) / 2.f;
        add_speed = (add_speed + push_speed) / 2.f;
    }
        //  calc dir
    if (new_dir != ZC_Vec3<float>())
    {
        add_speed *= time;  //  time control to changing current move speed
        float new_speed = ch_d.cur_move_speed + add_speed;
        float cos = ZC_Vec::Dot(ch_d.cur_move_direction, new_dir);
        if (cos < -0.3f)
        {
            if (new_speed < 0.f)
            {
                new_speed *= -1.f;
                    //  dir
                ch_d.cur_move_direction = (ch_d.cur_move_direction + new_dir) / 2.f;
                Normalize(ch_d.cur_move_direction, {});
            }
            ch_d.cur_move_speed = new_speed;
        }
        else
        {       //  dir
            ch_d.cur_move_direction = (ch_d.cur_move_direction + new_dir) / 2.f;
            Normalize(ch_d.cur_move_direction, {});
            ch_d.cur_move_speed = new_speed > max_move_speed ? max_move_speed : new_speed;
        }

        pos += ch_d.cur_move_direction * ch_d.cur_move_speed * time;     //  time control to move lenth
        assert(pos[0] * 0.f == 0.f || pos[1] * 0.f == 0.f || pos[2] * 0.f == 0.f);
    }
}

void G_OP_MarbleSphere::JumpMove(float time, ZC_Vec3<float>& pos)
{
    ZC_Vec3<float> push_dir;
    float push_speed = 0.f;
    float push_power = 0.f;     //  unused yet
    PushMove(push_dir, push_speed, push_power);
    if (push_speed != 0.f)
    {       // dir
        ch_d.cur_move_direction = (ch_d.cur_move_direction + push_dir) / 2.f;
        Normalize(ch_d.cur_move_direction, {});
            //  speed
        const float flight_push_coef = 2.f;
        push_speed *= flight_push_coef * time;  //  time control to changing current move speed
        ch_d.cur_move_speed += push_speed;  //  time control to changing current move speed
        if (ch_d.cur_move_speed > max_move_speed) ch_d.cur_move_speed = max_move_speed;
    }

    const float start_jump_angle = 90.f;    //  for cos
    const float jump_angle_power = 220.f;
    float jump_angle_coef = 2.f - ch_d.cur_move_speed / max_move_speed;  //  want to have with lower speed short fast jump
    ch_d.cur_jump_angle += jump_angle_power * jump_angle_coef * time;

    if (ch_d.cur_jump_angle >= 335.f || ch_d.cur_move_speed < 1.f)    //  finish jump
    {
        ch_d.space_position = SP_Flight;
        ch_d.cur_jump_angle = 0.f;
        return;
    }

    float flight_speed = ch_d.cur_move_speed * 2.f;
    float cos_flight_angle = std::cos(ZC_Vec::Radians(start_jump_angle - ch_d.cur_jump_angle));

    pos[0] += ch_d.cur_move_direction[0] * (push_speed == 0.f ? ch_d.cur_move_speed * time : push_speed);
    pos[1] += ch_d.cur_move_direction[1] * (push_speed == 0.f ? ch_d.cur_move_speed * time : push_speed);
    pos[2] += cos_flight_angle * flight_speed * time;

    assert(pos[0] * 0.f == 0.f || pos[1] * 0.f == 0.f || pos[2] * 0.f == 0.f);
}

void G_OP_MarbleSphere::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    if (!(static_cast<G_IObject*>(coll_result.pObj->GetHolder())->VGetTypeMask_IO() & G_OT__Ground)) return;

    ZC_Vec3<float> pos = this->VGetPosition_IO();
    if (coll_result.pushback != ZC_Vec3<float>())
    {
        pos += coll_result.pushback;    //push out of the object
        UpdateMatModel(pos);
    }

    if (!(coll_result.is_your_surface) && (*(coll_result.pSurf->normal))[2] == 0.f)   //  collision with the side surface of the platform
    {
        ch_d.on_ground_collision_in_prev_frame = false;
        if (!ch_d.pushed_from_platform)
        {
            this->upSK->SetVolume(G_SN__sphere_lands, ch_d.cur_move_speed / max_move_speed);    //  jump length dependth on speed, so landing sound power too
            this->upSK->SetSoundState(G_SN__sphere_lands, ZC_SS__Play);
        }
    }
    else    //  collision with up/down side of the platform
    {       //  determine, are sphere still on the platform (sphere's collision model not so spheric)
        const ZC_CO_FigureSphere& pFig_platform = coll_result.pObj->GetFigure();
        ZC_Vec3<float> dir_platform_center_to_shpere = pos - pFig_platform.center_fact;    //  vector from center of the platform to the center of the sphere
        ZC_Vec3<float> dir_platform_center_to_shpere_XY(dir_platform_center_to_shpere[0], dir_platform_center_to_shpere[1], 0.f);
        float centers_distance_XY = ZC_Vec::Length<float>(dir_platform_center_to_shpere_XY);  //  XY distance between platform and sphere
            //  find platform radius xy
        const ZC_Vec3<float>& surf_point = *((*(pFig_platform.GetSurfacesFact()))[0].points[0]);
        // const ZC_Vec3<float>& surf_point = *(pFig_platform.surfaces_fact[0].points[0]);     //  no metter wich point to take each will have same XY distance to platform center
        ZC_Vec3<float> point_center_dir = surf_point - pFig_platform.center_fact;   //  ditance vector from point to center (dir no metter it for length)
        float platfoorm_radius_XY = ZC_Vec::Length<float>({ point_center_dir[0], point_center_dir[1], 0.f });      //  not same radius as in pO_platform.radius!!!
        if (centers_distance_XY > platfoorm_radius_XY)   //  sphere out of platform but sphere's collision model don't get it to down, make some push
        {
            VPushObjectInDirection_IO(G_PushSet(dir_platform_center_to_shpere_XY, 1.f));
            ch_d.on_ground_collision_in_prev_frame = false;
            ch_d.pushed_from_platform = true;
        }
        else ch_d.on_ground_collision_in_prev_frame = true;
    }

    // if (ch_d.space_position != SP_Jump)  //  don't stop jump, caurse jump may have too small angle on start and still have collision with G_OT__Ground
    // {
    //     ch_d.space_position = SP_Ground;
    //         //  check is sphere out of the platform
    //     if (static_cast<G_Object*>(coll_result.pObj->GetHolder())->VGetTypeMask_IO() & G_OT__Ground)
    //     {       //  check by radius
    //             //  find distance between platform center and sphere center
    //         const ZC_CO_Figure& pFig_platform = coll_result.pObj->GetFigure();
    //         ZC_Vec3<float> dir_platform_center_to_shpere = pos - pFig_platform.center_fact;    //  vector from center of the platform to the center of the sphere
    //         ZC_Vec3<float> dir_platform_center_to_shpere_XY(dir_platform_center_to_shpere[0], dir_platform_center_to_shpere[1], 0.f);
    //         float centers_distance_XY = ZC_Vec::Length<float>(dir_platform_center_to_shpere_XY);  //  XY distance between platform and sphere
    //             //  find platform radius xy
    //         const ZC_Vec3<float>& surf_point = *(pFig_platform.surfaces_fact[0].points[0]);     //  no metter wich point to take each will have same XY distance to platform center
    //         ZC_Vec3<float> point_center_dir = surf_point - pFig_platform.center_fact;   //  ditance vector from point to center (dir no metter it for length)
    //         float platfoorm_radius_XY = ZC_Vec::Length<float>({ point_center_dir[0], point_center_dir[1], 0.f });      //  not same radius as in pO_platform.radius!!!
    //         if (centers_distance_XY > platfoorm_radius_XY)   //  sphere out of platform but spheres collision model don't get it to down, make some push
    //             VPushObjectInDirection_IO(G_PushSet(dir_platform_center_to_shpere_XY, 1.f));

    //             //  for quad platform
    //         // if (ZC_Vec::Length<float>({ plat_shpere_centers_dir[0], plat_shpere_centers_dir[1], 0.f }) > 0.7f)    //  sphere's center may be out of one of the platform edges
    //         // {       //  check platform side surfaces normals on out of
    //         //     for (const ZC_CO_Surface<ZC_Vec3<float>*>& surf : pFig_platform.surfaces_fact)
    //         //     {       //  side normal (not top or bottom)
    //         //         if ((*surf.normal)[2] == 0.f && ZC_DistanceFromPointToPlane(*(surf.points.front()), *(surf.normal), pos) > 0.f)
    //         //         {
    //         //             ch_d.space_position = SP_Flight;     //  spheres center still inside of the platdorm
    //         //             pos = ZC_Vec::MoveByLength(pos, *(surf.normal), 0.1f);    //  sphere out of platform, but don't want to fall, lets help them
    //         //         }
    //         //     }
    //         // }
    //     }
    // }

}

void G_OP_MarbleSphere::UpdateMatModel(const ZC_Vec3<float>& pos)
{
    this->VSetPosition_IO(pos);

    ZC_Mat4<float> model(1.f);
    model.Translate(pos);
    assert(model[3][0] * 0.f == 0.f || model[3][1] * 0.f == 0.f || model[3][2] * 0.f == 0.f || model[3][3] * 0.f == 0.f);

        //  rotate local_model matrix by xy if need
    if (ch_d.cur_move_direction != ZC_Vec3<float>())
    {
        static const float rotatte_move_speed_coef = 30.f;
        
        float rotate_angle_xy = ch_d.cur_move_speed * rotatte_move_speed_coef * ZC__FPS::GetPreviousFrameTime();    //  time control
        assert(ZC_Vec::Length(ch_d.cur_move_direction) > 0.999f && ZC_Vec::Length(ch_d.cur_move_direction) < 1.001f);     //  ch_d.cur_move_direction must be normalized!
        ZC_Mat4<float> rotate_model_xy(1.f);
        rotate_model_xy.Rotate(rotate_angle_xy, { - ch_d.cur_move_direction[1], ch_d.cur_move_direction[0], 0.f });    //  rotate only xy, but and dir can't have z != 0...
        ch_d.local_model = rotate_model_xy * ch_d.local_model;      //  can't be (ch_d.local_model *= model_new;) the order of multiplication is important!
    }
    model *= ch_d.local_model;

    this->upCO->UpdateModelMatrix(model);
}

bool G_OP_MarbleSphere::Normalize(ZC_Vec3<float>& vec, const ZC_Vec3<float>& vec_default)
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

void G_OP_MarbleSphere::UpdateColorDMG(float time)
{
    static const float seconds_dmg_phase = 2.f;

    if (ch_d.dmg_color == ZC_Vec3<float>() || this->changable_data_op.health <= 0.f) return;

    ch_d.dmg_time += time;
    if (ch_d.dmg_time >= seconds_dmg_phase)
    {
        ch_d.dmg_time = 0.f;
        this->unColor = 0.f;
        ch_d.dmg_color = {};
    }
    else this->unColor = G_InterpolateColor_PackToUInt(ch_d.dmg_color, { 0.01f, 0.01f, 0.01f }, ch_d.dmg_time / seconds_dmg_phase);
}

void G_OP_MarbleSphere::UpdateSound()
{
    if (ch_d.space_position_prev != ch_d.space_position)
    {
        switch (ch_d.space_position)
        {
        case SP_Ground:
        {
            this->upSK->SetSoundState(G_SN__sphere_flight, ZC_SS__Stop);

            this->upSK->SetVolume(G_SN__sphere_lands, ch_d.cur_move_speed / max_move_speed);    //  jump length dependth on speed, so landing sound power too
            this->upSK->SetSoundState(G_SN__sphere_lands, ZC_SS__Play);
            this->upSK->SetSoundState(G_SN__sphere_move, ZC_SS__PlayLoop);
        } break;
        case SP_Jump:
        {
            this->upSK->SetSoundState(G_SN__sphere_move, ZC_SS__Stop);

            this->upSK->SetSoundState(G_SN__sphere_jump, ZC_SS__Play);
            this->upSK->SetVolume(G_SN__sphere_flight, ch_d.cur_move_speed / max_move_speed);    //  jump length dependth on speed, so fly sound power too
            this->upSK->SetSoundState(G_SN__sphere_flight, ZC_SS__PlayLoop);
        } break;
        case SP_Flight:
        {
            this->upSK->SetSoundState(G_SN__sphere_move, ZC_SS__Stop);

            this->upSK->SetVolume(G_SN__sphere_flight, ch_d.cur_move_speed / max_move_speed);    //  jump length dependth on speed, so fly sound power too
            this->upSK->SetSoundState(G_SN__sphere_flight, ZC_SS__PlayLoop);

        } break;
        }
    }

    if (ch_d.space_position == SP_Ground)
        this->upSK->SetVolume(G_SN__sphere_move, ch_d.cur_move_speed / max_move_speed);
}




// void G_OP_MarbleSphere::SetMoveAndRotateSpeeds(float new_speed, float time)
// {
//     ch_d.cur_move_speed = new_speed;
    
//     ch_d.cur_rotattion_speed = ch_d.cur_move_speed * move_rotation_speed_coef * time * ch_d.rotation_signe_X;
//     float resutl = ch_d.rotate_angle_X + ch_d.cur_rotattion_speed;
//     ch_d.rotate_angle_X = resutl >= ZC_angle_360f ? resutl - ZC_angle_360f
//                     : resutl < 0.f ? ZC_angle_360f - resutl
//                     : resutl;
// }

// void G_OP_MarbleSphere::CalculateRotateZ(float time)
// {
//         //  rotate sphere origin to correct (new) direction
//     static const float rotate_power = 10.f;

//     if (ch_d.cur_move_speed == 0.f) return;

//     float cos_dir = ZC_Vec::Dot(ch_d.cur_move_direction, ch_d.cur_rotate_direction);
//     if (cos_dir < -0.8f)    //  dirs allmost oposite, help to rotate
//     {       //  avoid arthogonal rotation for X change movind direction. Object will change rotation_signe_X downer and start reverce rotate around X 
//         if ((ch_d.cur_rotate_direction[1] > 0.f && ch_d.cur_rotate_direction[1] >= 0.8f) || (ch_d.cur_rotate_direction[1] < 0.f && ch_d.cur_rotate_direction[1] <= -0.8f))
//             ch_d.cur_rotate_direction = {};
//         else    //  for Y axis changing orthogonal diraction make some help, rotate current rotate diraction on 1 degree (wrong way, but help)
//         {
//             ZC_Vec4<float> cur_rot_dir_v4 = ZC_Mat4<float>(1.f).Rotate(1.f, { 0.f, 0.f, -1.f }) * ZC_Vec4<float>(ch_d.cur_rotate_direction, 1.f);
//             ch_d.cur_rotate_direction = ZC_Vec3<float>(cur_rot_dir_v4[0], cur_rot_dir_v4[1], cur_rot_dir_v4[2]);
//         }
//     }

//     ZC_Vec3<float> cur_to_new_offset = ch_d.cur_move_direction - ch_d.cur_rotate_direction;   //  where to rotate from current rotation angle
//     ZC_Vec3<float> temp_rotate_direction = ch_d.cur_rotate_direction + cur_to_new_offset * rotate_power * time;   //  calculate new rotate diraction with time controll
//     if (temp_rotate_direction != ZC_Vec3<float>())
//     {          //  check normalization
//         Normalize(temp_rotate_direction, ch_d.cur_rotate_direction);
//         ch_d.cur_rotate_direction = temp_rotate_direction;
//     }
//     else return;    //  no rotation, out

//         //  calculate cos sin for rotation around Z axis
//     if (ch_d.cur_move_direction != ZC_Vec3<float>()) CalculateCosSin();
// }

// void G_OP_MarbleSphere::CalculateCosSin()
// {
//         //  take positive X for calculation.
//     ch_d.sin_Z = ZC_Vec::Dot({ 1.f, 0.f, 0.f }, ch_d.cur_rotate_direction);  //  usual Dot return cos, but now it sin, don't know why
//     ch_d.cos_Z = std::sqrt(1.f - (ch_d.sin_Z * ch_d.sin_Z));

//     assert(ch_d.cos_Z * 0.f == 0.f);
//         //  correct cos sin signes with axises of current roatate vector
//     if ((ch_d.cur_rotate_direction[0] >= 0.f && ch_d.cur_rotate_direction[1] >= 0.f) || (ch_d.cur_rotate_direction[0] <= 0.f && ch_d.cur_rotate_direction[1] >= 0.f))
//     {
//         ch_d.sin_Z *= -1.f;
//         ch_d.rotation_signe_X = -1.f;    //  x diractin rotation changed
//     }
//     else if ((ch_d.cur_rotate_direction[0] <= 0.f && ch_d.cur_rotate_direction[1] <= 0.f) || (ch_d.cur_rotate_direction[0] >= 0.f && ch_d.cur_rotate_direction[1] <= 0.f))
//     {
//         ch_d.sin_Z *= 1.f;
//         ch_d.cos_Z *= 1.f;
//         ch_d.rotation_signe_X = 1.f;    //  x diractin rotation changed
//     }
// }

// void G_OP_MarbleSphere::RotateZ_WithCosSin(ZC_Mat4<float>& model)
// {
//         //  Code taken from ZC_Mat4::Roate
//         //  Rotate model matri by Z axis using cos and sin calculated in G_OP_MarbleSphere::CalculateRotateZ()
//         //  Object start pos must be at {0,0,0} and look in {0,1,0} direction
//     ZC_Vec3<float> axis(0.f, 0.f, 1.f);
//     ZC_Vec3<float> temp(axis * (static_cast<float>(1) - ch_d.cos_Z));

//     ZC_Vec3<float> rotateX;
//     rotateX[0] = ch_d.cos_Z + temp[0] * axis[0];
//     rotateX[1] = temp[0] * axis[1] + ch_d.sin_Z * axis[2];
//     rotateX[2] = temp[0] * axis[2] - ch_d.sin_Z * axis[1];
//     ZC_Vec4<float> columnX = model[0] * rotateX[0] + model[1] * rotateX[1] + model[2] * rotateX[2];

//     ZC_Vec3<float> rotateY;
//     rotateY[0] = temp[1] * axis[0] - ch_d.sin_Z * axis[2];
//     rotateY[1] = ch_d.cos_Z + temp[1] * axis[1];
//     rotateY[2] = temp[1] * axis[2] + ch_d.sin_Z * axis[0];
//     ZC_Vec4<float> columnY = model[0] * rotateY[0] + model[1] * rotateY[1] + model[2] * rotateY[2];

//     ZC_Vec3<float> rotateZ;
//     rotateZ[0] = temp[2] * axis[0] + ch_d.sin_Z * axis[1];
//     rotateZ[1] = temp[2] * axis[1] - ch_d.sin_Z * axis[0];
//     rotateZ[2] = ch_d.cos_Z + temp[2] * axis[2];
//     ZC_Vec4<float> columnZ = model[0] * rotateZ[0] + model[1] * rotateZ[1] + model[2] * rotateZ[2];

//     model[0] = columnX;
//     model[1] = columnY;
//     model[2] = columnZ;
// }











// void G_OP_MarbleSphere::CalculateAngleRotate_angleZ(float time)
// {
    // static float cur_angle_Z = 0.f;
    // static ZC_Vec3<float> cur_rot_dir {0.f, 1.f, 0.f};
    // static const float speed_rotation_Z = 5.f;
//     auto p = ZC_Vec::Normalize(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(45.f, {0.f, 0.f, 1.f}) * ZC_Vec4<float>(0.f, 1.f, 0.f, 1.f)));

//     static const ZC_Vec3<float> start_front_dir(0.f, 1.f, 0.f);
//     if (cur_rot_dir == ch_d.cur_move_direction || ch_d.cur_move_direction == ZC_Vec3<float>()) return;
//     float cos_new_dir = ZC_Vec::Dot(ch_d.cur_move_direction, cur_rot_dir);
//     if (cos_new_dir > 1.f) cos_new_dir = 1.f;
//     else if (cos_new_dir < -1.f) cos_new_dir = -1.f;
//         assert(cos_new_dir * 0.f == 0.f);
//     float angle_rotate_on = ZC_Vec::Degrees(std::acos(cos_new_dir));
//         assert(angle_rotate_on * 0.f == 0.f);
//     float dist_to_cur_angle = ZC_Vec::Length(start_front_dir - cur_rot_dir);
//         assert(dist_to_cur_angle * 0.f == 0.f);
//     float dist_to_new_angle = ZC_Vec::Length(start_front_dir - ch_d.cur_move_direction);
//         assert(dist_to_new_angle * 0.f == 0.f);

//     // if (cos_new_dir < 0.f)
//     // {
//     //     signe *= -1.f;
//     //     ch_d.rotation_signe_X *= -1.f;  //  change diraction
//     // }
//     // if (cos_new_dir >= 0.f)   //  save move rotation diration, just rotate around Z
//     // {
//     //     if (ch_d.cur_move_direction[0] <= 0)
//     //     {
//     //         if (dist_to_cur_angle > dist_to_new_angle)      //  clockwise rotation
//     //         {
//     //             cur_angle_Z -= angle_rotate_on * speed_rotation_Z * time;
//     //         }
//     //         else if (dist_to_cur_angle < dist_to_new_angle)     // counterclockwise rotation
//     //         {
//     //             cur_angle_Z += angle_rotate_on * speed_rotation_Z * time;
//     //         }
//     //         else    //  equal
//     //         {

//     //         }
//     //     }
//     //     else
//     //     {
//     //         if (cur_rot_dir[0] <= 0.f && cur_rot_dir[1] >= 0)
//     //         {
//     //             cur_angle_Z -= angle_rotate_on * speed_rotation_Z * time;
//     //         }
//     //         else if (cur_rot_dir[0] <= 0.f && cur_rot_dir[1] >= 0)
//     //         {
//     //             cur_angle_Z += angle_rotate_on * speed_rotation_Z * time;
//     //         }
//     //         else
//     //         {
//     //             if (dist_to_cur_angle > dist_to_new_angle)      //  clockwise rotation
//     //             {
//     //                 cur_angle_Z += angle_rotate_on * speed_rotation_Z * time;
//     //             }
//     //             else if (dist_to_cur_angle < dist_to_new_angle)     // counterclockwise rotation
//     //             {
//     //                 cur_angle_Z -= angle_rotate_on * speed_rotation_Z * time;
//     //             }
//     //         }
//     //     }
//     // }
//     // else    //  change move rotate diration (reverse rotation)
//     // {
//     //     ch_d.rotation_signe_X *= -1.f;  //  change diraction

//     //     // if (dist_to_cur_angle > dist_to_new_angle)      //  clockwise rotation
//     //     // {
//     //     //     cur_angle_Z -= angle_rotate_on * speed_rotation_Z * time;
//     //     // }
//     //     // else if (dist_to_cur_angle < dist_to_new_angle)     // counterclockwise rotation
//     //     // {
//     //     //     cur_angle_Z += angle_rotate_on * speed_rotation_Z * time;
//     //     // }
//     //     // else    //  equal
//     //     // {

//     //     // }
//     //     // cur_angle_Z *= -1.f;
//     // }
//     if (cur_angle_Z > 360.f) cur_angle_Z -= 360.f;
//     else if (cur_angle_Z < -360.f) cur_angle_Z += 360.f;

//     cur_rot_dir = ZC_Vec::Normalize(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(cur_angle_Z, { 0.f, 0.f, 1.f }) * start_front_dir));

//     // std::cout<<"cur_rot_dir {"<<cur_rot_dir[0]<<cur_rot_dir[1]<<"}     cur_angle_Z = "<<cur_angle_Z<<"      "

//     // float cos = ZC_Vec::Dot(cur_rot_dir, ch_d.cur_move_direction);
//     // float sin = 1.f - (cos * cos);

//     // float angle = ZC_Vec::Degrees(std::acos(cos));
//     // // if (cur_angle_Z)

//     // float cos_dir = ZC_Vec::Dot(ch_d.cur_move_direction, ch_d.cur_rotate_direction);
//     // float angle_dir = ZC_Vec::Degrees(std::acos(cos_dir));
//     // std::cout<<cos<<std::endl;
// }


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







// void G_OP_MarbleSphere::UpdateRotateAxis(int dir_mask, float time)
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





// void G_OP_MarbleSphere::UpdateModelWithLeftOrRightRotation(float time, bool move_to_left, bool update_pos)
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

// void G_OP_MarbleSphere::MakeFrontOrBackMove(const ZC_Vec3<float>& offset, float rotate)
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