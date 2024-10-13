#include "G_Platform.h"

#include <Model/G_Models.h>
#include <ZC/Video/ZC_SWindow.h>

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, nullptr)
{}

G_Platform::~G_Platform()
{
    ecUpdater.Disconnect();
    
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
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_IO(this->upCO->GetFigure().center_fact, angle);
}

bool G_Platform::SwitchWithWinPlatform(G_Platform* pPLat_win)
{
    if (!objects_on_platform.empty()) return false; //  platfrom not empty can't switch
    {
        platf_trans.SwitchMoselMatrices(pPLat_win->platf_trans);
        return true;
    }
}

void G_Platform::RotateExternal(float angle)
{
    platf_trans.Update_rotate_angle_external_Z(platf_trans.rotate_angle_external_Z + angle);
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_IO({ 0.f, 0.f, 0.f }, angle);
}

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans, G_ModelName modelName, int texSetId, ZC_uptr<G_GameSoundSet>&& _upSK)
    : G_Object(modelName, texSetId, new ZC_CollisionObject(G_Models::GetModel_COFigure(modelName, _plat_trans.scale), ZC_C0_Type::ZC_COT__StaticSoloCollision, this,
        { &G_Platform::Callback_Collision, this }, _plat_trans.CalculateModelMatrix()), std::move(_upSK)),
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
                    VAddObjectOnPlatform(pObj_new);     //  try to add object on platform
                        //  change platforms owner
                    op.pPlatf = this;   //  change owner in static list to us
                }
                return;
            }
        }
            //  object are not in platforms lists, add it
        objAndPlatforms.emplace_back(ObjAndPlatform{ .pObj_pushable = pObj_new, .pPlatf = this });  //  add to global list
        VAddObjectOnPlatform(pObj_new);     //  try to add object on platform
    }
}

bool G_Platform::IsObjectInPlatformRadiusXY(G_Object* pObj)
{
    ZC_Vec3<float> platform_center = this->upCO->GetFigure().center_fact;   //  PLATFORM POS HERE, NOT IN platf_trans.translate, rotation chage position of the platform too
    ZC_Vec3<float> obj_pos = pObj->VGetPosition_IO();
    float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_center[0], platform_center[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
    return distance <= pObj->upCO->GetFigure().radius + this->upCO->GetFigure().radius;
}

void G_Platform::VAddObjectOnPlatform(G_Object* pObj_add)
{
    objects_on_platform.emplace_back(pObj_add);
}