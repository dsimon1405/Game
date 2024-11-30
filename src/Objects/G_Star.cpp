#include "G_Star.h"

#include <Map/G_Map.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <Model/G_Models.h>
#include <System/G_RenderLevel.h>
#include <System/G_LightUBO.h>

G_Star::G_Star()
    : particles_star(1000, {}, star_scale + 1.f, 30.f),
    ec_updater(ZC__Updater::Connect({ &G_Star::Callback_Updater, this }, G_UL__game_play)),
    collision_object(new ZC_CO_FigureSphere({0,0,0}, 10), ZC_COT__Static, this, { &G_Star::Callback_Collision, this }),
    ss_star(GetSoundsVector()),
    ds_con(G_Models::GetModel_DSController(G_MN__SphereStar, 0))
{
    ds_con.SetUniformsData(ZC_UN_unModel, collision_object.GetModelMatrix());
    ds_con.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_ParticleStar);
    
    particles_star.SetAlpha(0.1);
    particles_star.SetDrawState(true);
}

G_Star::~G_Star()
{
    ec_updater.Disconnect();
}

void G_Star::SetNewPosition(float dist_to_star)
{
    dist_to_center = dist_to_star;
    collision_object.UpdateRadius(dist_to_star / 2.f);      //  will start damage object on that distance
    cur_angle_Z = ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i);
    CalculateModelMatrix(1.f);
}

float G_Star::VGetRadius_IO() const noexcept
{
    return collision_object.GetFigure().radius;
}

void G_Star::VSetDefaultState_IO()
{       //  stupid reset
    static const ZC_Vec3<float> temp_pos(0.f, 0.f, 1000.f);
    ss_star.SetPosition(&temp_pos);     //  origin in collision object at first game will be (0,0,0), collision not happend yet to move us. So need set temp pos somwhere to make sond set connected to cam pos updater but without cam distance volume 0
    ss_star.SetDefaultState();
    ss_star.SetSoundState(G_SN__star, ZC_SS__PlayLoop);
    ss_star.SetPosition(&(collision_object.GetFigure().center_fact));
}

std::vector<G_GameSound> G_Star::GetSoundsVector()
{
    std::vector<G_GameSound> v;
    v.reserve(1);
    v.emplace_back(G_GameSound(G_SN__star));
    return v;
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
    // UpdateDMG(time);
    // CalculateModelMatrix(time);


static bool b = true;
    if (b)
    {
        CalculateModelMatrix(time);
        b = false;
        this->ds_con.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_None);
        this->particles_star.SetDrawState(false);
    }
    else
    {
    static const uint color_packed = ZC_PackColorFloatToUInt_RGB(1.f, 0.86f, 0.55f);    //  peach color
    G_LightUBO::UpdateLightData(G_LN__Star, G_LightSet{ .pos = collision_object.GetFigure().center_fact, .color = color_packed });
    }
}

void G_Star::Callback_Collision(const ZC_CO_CollisionResult& coll_result)
{
    G_IObject* pObject_dmg(static_cast<G_IObject*>(coll_result.pObj->GetHolder()));
    if (pObject_dmg->VGetTypeMask_IO() & G_OT__Damageable && !ZC_Find(objects_dmg, pObject_dmg))   //  add damdagable, not added yet object
        objects_dmg.emplace_back(pObject_dmg);
}

void G_Star::UpdateDMG(float time)
{
    static const float seconds_between_dmg = 1.f;
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
                    static const float max_dmg = 50.f;     //  dmg at center of sphere

                    float penetration = radiuses_sum - centers_dist;
                    float dmg_coef = penetration / VGetRadius_IO();
                    float dmg = max_dmg * dmg_coef;
                    (*iter)->VDamageObject_IO(dmg, G_OT__Star);
                    ++iter;
                }
            }
            dmg_time = 0.f;
        }
    }
    else dmg_time = 0.f;
}

void G_Star::CalculateModelMatrix(float time)
{
    static const float rot_speed_XY = 60.f;
    static const float rot_speed_Z = 3.f;

    cur_angle_XY += rot_speed_XY * time;
    cur_angle_Z += rot_speed_Z * time;
    ZC_Mat4<float> model = ZC_Mat4<float>(1.f).Rotate(cur_angle_Z, {0.f, 0.f, 1.f}).Translate({ 0.f, dist_to_center, 10.f })
        .Rotate(cur_angle_XY, { 1.f, 1.f, 0.f }).Scale(star_scale, star_scale, star_scale);
    ZC_Vec3<float> star_center = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, 0.f, 0.f, 1.f));

        //  update pos in collision, particles, and light
    collision_object.UpdateModelMatrix(model);
    particles_star.SetPosition(star_center);
    static const uint color_packed = ZC_PackColorFloatToUInt_RGB(1.f, 0.86f, 0.55f);    //  peach color
    G_LightUBO::UpdateLightData(G_LN__Star, G_LightSet{ .pos = star_center, .color = color_packed });
}