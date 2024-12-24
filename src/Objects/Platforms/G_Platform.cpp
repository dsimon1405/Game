#include "G_Platform.h"

#include <Model/G_Models.h>
#include <ZC/ZC__System.h>
#include <Map/G_Map.h>
#include <ZC/Objects/Camera/ZC_Camera.h>

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, nullptr, G_PT__Start)
{}

G_Platform::~G_Platform()
{
    ecUpdater.Disconnect();
    
    light_holder.DeactivatePlatformLight(this);

    if (objAndPlatforms.empty()) return;
    for (auto iter = objAndPlatforms.begin(); iter != objAndPlatforms.end(); ++iter)
    {
        if (iter->pPlatf == this)
        {
            objAndPlatforms.erase(iter);
            break;
        }
    }
}

void G_Platform::RotateInternal(float angle)
{
    platf_trans.Update_rotate_angle_internal_Z(platf_trans.rotate_angle_internal_Z + angle);
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_IO(this->upCO->GetCenterFact(), angle);
}

void G_Platform::RotateExternal(float angle)
{
    platf_trans.Update_rotate_angle_external_Z(platf_trans.rotate_angle_external_Z + angle);
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_IO({ 0.f, 0.f, 0.f }, angle);
}

bool G_Platform::SwitchWithWinPlatform(G_Platform* pPLat_win)
{
    if (!objects_on_platform.empty()) return false; //  platfrom not empty can't switch
    {
        platf_trans.SwitchMoselMatrices(pPLat_win->platf_trans);
        return true;
    }
}

G_LightSet G_Platform::GetLightSet_P()
{
    assert(!objects_on_platform.empty());
        //  when object at the edge of the platform, need to make effect there is no light farther. So dive radous on some value, I found 9.2F.
    static const float light_platform_radius = G_Map::radiusXY_other_platforms - (G_Map::radiusXY_other_platforms / platform_light_radius_divisor);
    return G_LightSet{ .pos = CalculateLightPos(light_platform_radius), .color = this->unColor };
}

void G_Platform::ChekDrawState()
{
    const ZC_Vec3<float>& platf_center = this->upCO->GetCenterFact();
    const ZC_Mat4<float>& persp_view = *(ZC_Camera::GetActiveCamera()->GetPerspectiveView());

    auto lamb_is_in_frustum = [this](const ZC_Vec4<float>& display_pos)
    {
        if (display_pos[3] <= 0.f) return false;  //  if < 0 point behind the camera, if == 0 point equal cam pos. Don't draw in each case
        float display_x = display_pos[0] / display_pos[3];      //  horizontal, range [-1, 1]
        float display_y = display_pos[1] / display_pos[3];      //  vertical, range [-1, 1]
        float display_z = display_pos[2] / display_pos[3];      //  depth, range [0, 1]
        if (display_x >= -1.f && display_x <= 1.f && display_y >= -1.f && display_y <= 1.f && display_z >= 0.f && display_z <= 1.f)
        {
            VSetDrawState(true);
            return true;
        }
        return false;
    };

    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] + G_Map::radiusXY_other_platforms, platf_center[1] + G_Map::radiusXY_other_platforms, platf_center[2] + G_Map::platforms_all_half_height_Z, 1.f))) return;     //  tr near
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] + G_Map::radiusXY_other_platforms, platf_center[1] - G_Map::radiusXY_other_platforms, platf_center[2] + G_Map::platforms_all_half_height_Z, 1.f))) return;     //  tr far
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] - G_Map::radiusXY_other_platforms, platf_center[1] + G_Map::radiusXY_other_platforms, platf_center[2] + G_Map::platforms_all_half_height_Z, 1.f))) return;     //  tl near
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] - G_Map::radiusXY_other_platforms, platf_center[1] - G_Map::radiusXY_other_platforms, platf_center[2] + G_Map::platforms_all_half_height_Z, 1.f))) return;     //  tl far
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] + G_Map::radiusXY_other_platforms, platf_center[1] + G_Map::radiusXY_other_platforms, platf_center[2] - G_Map::platforms_all_half_height_Z, 1.f))) return;     //  br near
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] + G_Map::radiusXY_other_platforms, platf_center[1] - G_Map::radiusXY_other_platforms, platf_center[2] - G_Map::platforms_all_half_height_Z, 1.f))) return;     //  br far
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] - G_Map::radiusXY_other_platforms, platf_center[1] + G_Map::radiusXY_other_platforms, platf_center[2] - G_Map::platforms_all_half_height_Z, 1.f))) return;     //  bl near
    if (lamb_is_in_frustum(persp_view * ZC_Vec4<float>(platf_center[0] - G_Map::radiusXY_other_platforms, platf_center[1] - G_Map::radiusXY_other_platforms, platf_center[2] - G_Map::platforms_all_half_height_Z, 1.f))) return;     //  bl far
    VSetDrawState(false);


        //  Tryied to made collision frustum with sphere model by radius
    // void G_Platform::ChekDrawState(const ZC_Vec3<float>& cam_pos, const ZC_Vec3<float>& cam_front_norm, const ZC_Mat4<float>& persp_view)

    // float platf_radius = 10.f;  //  works only with too large radius
    // // float platf_radius = this->upCO->GetFigure().radius;
    // ZC_Vec3<float> dir_cam_to_platf = platf_center - cam_pos;
    // float dist_platf_to_cam = ZC_Vec::Length(dir_cam_to_platf);   //  hypotenuse
    // if (dist_platf_to_cam <= platf_radius)      //  cam in platfrom radius, so need draw
    // {
    //     VSetDrawState(true);
    //     return;
    // }

    //     //  need to build perpendicular from platform center to cam_front line. We know hypotenuse, need to know katet from cam_pos to perpendicular.
    // ZC_Vec3<float> cam_to_platf_norm = ZC_Vec::Normalize(dir_cam_to_platf);
    // float cos_cam_pos_angle = ZC_Vec::Dot(cam_front_norm, cam_to_platf_norm);
    // if (cos_cam_pos_angle > 0)
    // {
    //     float dist_cam_to_perpendicular = cos_cam_pos_angle * dist_platf_to_cam;   //  adjacent leg = cons(adjacent angle) * hypotenuse
    //     ZC_Vec3<float> perpendicular_pos = ZC_Vec::MoveByLength(cam_pos, cam_front_norm, dist_cam_to_perpendicular);
    //     ZC_Vec3<float> closest_point_to_frustum = ZC_Vec::MoveByLength(platf_center, perpendicular_pos - platf_center, platf_radius);
    //     ZC_Vec4<float> display_pos = persp_view * ZC_Vec4<float>(closest_point_to_frustum, 1.f);
    //     if (display_pos[3] <= 0.f) VSetDrawState(false);  //  if < 0 point behind the camera, if == 0 point equal cam pos. Don't draw in each case
    //     else
    //     {
    //         float display_x = display_pos[0] / display_pos[3];      //  horizontal, range [-1, 1]
    //         float display_y = display_pos[1] / display_pos[3];      //  vertical, range [-1, 1]
    //         float display_z = display_pos[2] / display_pos[3];      //  depth, range [0, 1]
    //         VSetDrawState(display_x >= -1.f && display_x <= 1.f && display_y >= -1.f && display_y <= 1.f && display_z >= 0.f && display_z <= 1.f);
    //     }
    // }
    // else VSetDrawState(false);      //  angle betwwen cam_front and platform in range [90, 270], can't see platform 
}

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans, G_ModelName modelName, int texSetId, ZC_uptr<G_GameSoundSet>&& _upSK, G_PlatformType _platform_type)
    : G_Object(modelName, texSetId, new ZC_CollisionObject(G_Models::GetModel_COFigure(modelName, _plat_trans.scale), ZC_C0_Type::ZC_COT__StaticSoloCollision, this,
        { &G_Platform::Callback_Collision, this }, _plat_trans.CalculateModelMatrix()), std::move(_upSK)),
    platform_type(_platform_type),
    platf_trans(_plat_trans)
{
    platf_trans.pCO = this->upCO.Get();
}

G_ObjectTypeMask G_Platform::VGetTypeMask_IO() const
{
    return G_OT__Ground | G_OT__Platform;
}

void G_Platform::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    G_Object* pObj_new = static_cast<G_Object*>(coll_result.pObj->GetHolder());
    if (pObj_new->VGetTypeMask_IO() & G_OT__Pushable)   //  object is pushable may be added to object on platform
    {
        for (ObjAndPlatform& op : objAndPlatforms)
        {
            if (op.pObj_pushable == pObj_new)   //  object allready was at some platform
            {
                if (op.pPlatf != this)  //  platform changed to current platform
                {
                        //  change previous platform state
                    std::erase(op.pPlatf->objects_on_platform, pObj_new);   //  erase from list of previous owners
                    if (op.pPlatf->objects_on_platform.empty()) op.pPlatf->VDeactivatePlatform_P();    //  previous owner have no more objects, deactivate
                        //  change curernt platform state
                    VAddObjectOnPlatform_P(pObj_new);     //  try to add object on platform
                    light_holder.SetLightHolder(this);  //  change light holder
                        //  change platforms owner
                    op.pPlatf = this;   //  change owner in static list on current platform
                }
                return;
            }
        }
            //  object are not in platforms lists, add it
        objAndPlatforms.emplace_back(ObjAndPlatform{ .pObj_pushable = pObj_new, .pPlatf = this });  //  add to global list
        VAddObjectOnPlatform_P(pObj_new);     //  try to add object on platform
        light_holder.SetLightHolder(this);  //  change light holder
    }
}

bool G_Platform::IsObjectInPlatformRadiusXY(G_Object* pObj)
{
    ZC_Vec3<float> platform_center = this->upCO->GetCenterFact();   //  PLATFORM POS HERE, NOT IN platf_trans.translate, rotation chage position of the platform too
    ZC_Vec3<float> obj_pos = pObj->VGetPosition_IO();
    float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_center[0], platform_center[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
    return distance <= pObj->upCO->GetFigure().radius + this->upCO->GetFigure().radius;
}

ZC_Vec3<float> G_Platform::CalculateLightPos(float platform_radius_divided_xy)
{
        //  current system made for only 1 object, for more objects need some else system
    const ZC_Vec3<float>& obj_center = objects_on_platform.front()->upCO->GetCenterFact();
    const ZC_Vec3<float>& platf_center = this->upCO->GetCenterFact();
    ZC_Vec3<float> dir_platf_center_to_obj_center = obj_center - platf_center;
        //  calculate xy
    ZC_Vec2<float> dir_platf_center_to_obj_center_xy = ZC_Vec::Vec3_to_Vec2(dir_platf_center_to_obj_center);
    float length_xy = ZC_Vec::Length(dir_platf_center_to_obj_center_xy);
    ZC_Vec2<float> light_pos_xy = length_xy <= platform_radius_divided_xy ? ZC_Vec::Vec3_to_Vec2(obj_center)   //  xy in platform radius
        : ZC_Vec::MoveByLength(ZC_Vec::Vec3_to_Vec2(platf_center), dir_platf_center_to_obj_center_xy, platform_radius_divided_xy);   //  move platf_center to platf border by xy
        //  calculate z
    float dir_platf_center_to_obj_center_z = std::abs(dir_platf_center_to_obj_center[2]);   //  dir could be negative, need length
    float move_length_z = dir_platf_center_to_obj_center_z > G_Map::platforms_all_half_height_Z ? G_Map::platforms_all_half_height_Z : dir_platf_center_to_obj_center_z; //  light can't be ouut of platf_half_height_z
    if (dir_platf_center_to_obj_center[2] < 0.f) move_length_z *= -1.f;  //  take signe of the direction
    float light_pos_z = platf_center[2] + move_length_z;

    ZC_Vec3<float> light_pos_on_platf_surface(light_pos_xy[0], light_pos_xy[1], light_pos_z);   //  poisitoin on the one of the edges of the platform, but it's too close to the under light object
    
    const float light_start_offset = 3.f;   //  for platforms light pos farther light power must be started (to improve diffuse effect).
    return ZC_Vec::MoveByLength(light_pos_on_platf_surface, light_pos_on_platf_surface - obj_center, light_start_offset);   //  move light from object
}

std::list<G_Object*>::iterator G_Platform::EraseObjectFromPlatform(std::list<G_Object*>::iterator iter)
{
    light_holder.DeactivatePlatformLight(this); 
    return objects_on_platform.erase(iter);
}

void G_Platform::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    objects_on_platform.emplace_back(pObj_add);
}

void G_Platform::VSetDrawState(bool need_draw)
{
    this->dsCon.SwitchToDrawLvl(ZC_RL_Default, need_draw ? ZC_DL_Drawing : ZC_DL_None);
}