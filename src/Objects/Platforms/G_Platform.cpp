#include "G_Platform.h"

#include <Model/G_Models.h>
#include <ZC/Video/ZC_SWindow.h>
    // void G_Platform::CallbackTestRotate(float time)
    // {
    //     static float degrees = 10;
    //     static float second_on_rotate = 1;

    //     if (!objects_on_platform.empty())
    //     {
    //         float angle = degrees * time / second_on_rotate;
    //         platf_trans.Update_rotate_angle_internal_Z(platf_trans.rotate_angle_internal_Z + angle);

    //         objects_on_platform.front()->VOnGroundRotateZ_O({0,0,0}, angle);
    //     }
    // }

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0)
{
    // ZC_SWindow::ConnectToUpdater({ &G_Platform::CallbackTestRotate, this }, 0);
}

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
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_O(platf_trans.translate, angle);
}

void G_Platform::RotateExternal(float angle)
{
    platf_trans.Update_rotate_angle_external_Z(platf_trans.rotate_angle_external_Z + angle);
    for (G_Object* pObj : objects_on_platform) pObj->VOnGroundRotateZ_O({ 0.f, 0.f, 0.f }, angle);
}

G_Platform::G_Platform(const G_PlatformTransforms& _plat_trans, G_ModelName modelName, int texSetId)
    : G_Object(modelName, texSetId, new ZC_CollisionObject(G_Models::GetModel_COFigure(modelName, _plat_trans.scale), ZC_C0_Type::ZC_COT__StaticSoloCollision, this,
        { &G_Platform::Callback_Collision, this }, _plat_trans.CalculateModelMatrix())),
    platf_trans(_plat_trans)
{
    platf_trans.pCO = this->upCO.Get();
}

G_ObjectTypeMask G_Platform::VGetType_O() const
{
    return G_OT__Ground;
}

void G_Platform::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    G_Object* pObj_new = static_cast<G_Object*>(coll_result.pObj->GetHolder());
    if (pObj_new->VGetType_O() & G_OT__Pushable)   //  object is pushable may be added to object on platform
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
    ZC_Vec3<float> obj_pos = pObj->VGetPosition_O();
    float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_center[0], platform_center[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
    return distance <= pObj->upCO->GetFigure().radius + this->upCO->GetFigure().radius;
}

void G_Platform::VAddObjectOnPlatform(G_Object* pObj_add)
{
    objects_on_platform.emplace_back(pObj_add);
}