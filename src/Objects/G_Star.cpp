#include "G_Star.h"

#include <GamePlay/G_Map.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/OpenGL/Buffer/ZC_UBOs.h>

G_Star::G_Star()
    : particles_star(2000, {}, G_Map::radiusXY_other_platforms / 3.f, 30.f),
    ec_updater(ZC_SWindow::ConnectToUpdater({ &G_Star::Callback_Updater, this }, G_UL__game_play)),
    collision_object(new ZC_CO_FigureSphere({50, 50, 50}, 10), ZC_COT__Static, this, { &G_Star::Callback_Collision, this }),
    ubo_light()
{
    particles_star.SetDrawState(true);
}

G_Star::~G_Star()
{
    ec_updater.Disconnect();
}

void G_Star::SetNewPosition(float dist_to_star)
{       //  random start position
    ZC_Vec3<float> start_pos(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i), { 0.f, 0.f, 1.f })
        * ZC_Vec4<float>(0.f, dist_to_star, 0.f, 1.f)));
    
        //  update pos in collisoin and particles
    collision_object.UpdateModelMatrix(ZC_Mat4(1.f).Translate(start_pos));
    collision_object.UpdateRadius(dist_to_star / 3.f);      //  will start damage object on that distance

    particles_star.SetPosition(start_pos);
}

float G_Star::VGetRadius_IO() const noexcept
{
    return collision_object.GetFigure().radius;
}

ZC_Vec3<float> G_Star::VGetPosition_IO() noexcept
{
    return particles_star.GetPosition();
}

G_ObjectTypeMask G_Star::VGetTypeMask_IO() const
{
    return G_OT__Star;
}

void G_Star::Callback_Updater(float time)
{
    UpdateDMG(time);
    
    // static const float rotate_speed = 5.f;   //  5 degree / 1 second

    // ZC_Vec3<float> new_pos(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(rotate_speed * time, { 1.f, 0.f, 1.f }) * VGetPosition_IO()));
    // collision_object.UpdateModelMatrix(ZC_Mat4(1.f).Translate(new_pos));
    // particles_star.SetPosition(new_pos);
}

void G_Star::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    G_IObject* pObject_dmg(static_cast<G_IObject*>(coll_result.pObj->GetHolder()));
    if (pObject_dmg->VGetTypeMask_IO() & G_OT__Damageable && !ZC_Find(objects_dmg, pObject_dmg))   //  add damdagable, not added yet object
        objects_dmg.emplace_back(pObject_dmg);
}

void G_Star::UpdateDMG(float time)
{
    static const float seconds_between_dmg = 3.f;
    static float dmg_time = 0;
    
        //  dmg
    if (!(objects_dmg.empty()))
    {
        dmg_time += time;
        if (dmg_time >= seconds_between_dmg)    //  make dmg
        {
            for (auto iter = objects_dmg.begin(); iter != objects_dmg.end(); )
            {
                float centers_dist = ZC_Vec::Length(VGetPosition_IO() - (*iter)->VGetPosition_IO());
                float radiuses_sum = VGetRadius_IO() + (*iter)->VGetRadius_IO();
                if (centers_dist > radiuses_sum) iter = objects_dmg.erase(iter);    //  object leaved
                else
                {
                    static const float max_dmg = 100.f;     //  dmg at center of sphere

                    float penetration = radiuses_sum - centers_dist;
                    float dmg_coef = penetration / VGetRadius_IO();
                    float dmg = max_dmg * dmg_coef;
                    // (*iter)->VDamageObject_IO(dmg, G_OT__Star);
                }
            }
        }
    }
    else dmg_time = 0.f;
}