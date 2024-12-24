#include "G_Star.h"

#ifdef G_NewV
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <System/G_RenderLevel.h>
#include <System/G_LightUBO.h>
#include <ZC/File/ZC_File.h>

G_Star::G_Star()
    : ps_star(
        G_PS_Source
            {
                .particles_count = 10,
                .render = G_PS_Source::Render
                    {
                        .render_level = ZC_RL_Default,
                        .drawer_level = G_DL_AlphaBlending_ParticleSystem_FlameStar,
                        .tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/textures/flame.jpg").string(),
                        .create_apha_from_rgb = true,
                        .columns_count = 4,
                        .rows_count = 4,
                    },
                .spawn_shape = G_PS_Source::SpawnShape
                    {
                        .shape = G_PS_Source::SpawnShape::S__Sphere,
                        .fill_to_center = 0.7f
                    },
                .spawn_mat_model = G_PS_Source::SpawnMatModel
                    {
                        // .translate = { 20.f, 20.f, 10.f },
                        // .scale = { 1.f, 1.f, 1.f }
                    },
                .size = G_PS_Source::Size
                    {
                        .width = 30.f,
                        .height = 30.f,
                    },
                .life_time = G_PS_Source::LifeTime
                    {
                        .secs_to_start_max = 1.f,
                        .secs_min = 2.f,
                        .secs_max = 3.f
                    },
                .move = G_PS_Source::Move
                    {
                        .direction_type = G_PS_Source::Move::DT__from_particles_center,
                        .speed_power = 1.f,
                        .speed_min = 3.f,
                        .speed_max = 5.f
                    },
                .rotate = G_PS_Source::Rotate
                    {
                        .angle_use = G_PS_Source::Rotate::AU__random_between_constants,
                        .angle_1 = -45.f,
                        .angle_2 = 45.f,
                    },
                .animation = G_PS_Source::Animation
                    {
                        .repaet = G_PS_Source::Animation::R_Loop,
                        .tiles_per_second = 7.f,
                    },
                .color = G_PS_Source::Color
                    {
                        .rgb_use = G_PS_Source::Color::RGBU_Add,
                        .appear_secs = 0.2f,
                        .disappear_secs = 0.6f,
                        .rgba_start = { 1.f, 1.f, 1.f, 0.f },
                        .rgba_end = { 0.f, 0.f, 0.f, 0.f },
                    },
            },
        {}),
    ec_updater(ZC__Updater::Connect({ &G_Star::Callback_Updater, this }, G_UL__game_play)),
    collision_object(new ZC_CO_FigureSphere({0,0,0}, 10), ZC_COT__Static, this, { &G_Star::Callback_Collision, this }),
    ss_star(GetSoundsVector())
{
    ps_star.SetDrawState(true);
}

G_Star::~G_Star()
{
    ec_updater.Disconnect();
}

void G_Star::SetNewPosition(float dist_to_star)
{
    static const float speed_horizontal_move = 8.f;     //  speed of movement around the map circle

    float new_length = 2.f * ZC_PI * dist_to_star;
    rot_angle_Z = ZC_angle_360f / (new_length / speed_horizontal_move);
    bool clockwise_move = ZC_Random::GetRandomInt(0, 1);
    if (clockwise_move) rot_angle_Z *= -1.f;
    cur_angle_Z = ZC_Random::GetRandomInt(ZC_angle_0i, ZC_angle_360i);

    if (dist_to_star != dist_to_center)     //  recalculate light data
    {
        light_linear_start_player_sphere = G_LightUBO::c_light.attenuations[G_LAN_PlayerSphere].linear;
        light_linear_start_platform = G_LightUBO::c_light.attenuations[G_LAN_Platform].linear;
        light_linear_start_cube_map = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear;
        RecalcLightData();
    }

    dist_to_center = dist_to_star;
    collision_object.UpdateRadius(dist_to_star / 2.f);      //  will start damage object on that distance
}

float G_Star::VGetRadius_IO() const noexcept
{
    return collision_object.GetFigure().radius;
}

void G_Star::VSetDefaultState_IO()
{       //  stupid reset
    static const ZC_Vec3<float> temp_pos(0.f, 0.f, 1000.f);
    ss_star.SetPosition(&temp_pos);     //  origin in collision object at first game will be (0,0,0), collision not happend yet to move us. So need set temp pos somwhere to make sound set connected to cam pos updater but without cam distance volume 0
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
    return collision_object.GetFigure().center_fact;
}

G_ObjectTypeMask G_Star::VGetTypeMask_IO() const
{
    return G_OT__Star;
}

void G_Star::Callback_Updater(float time)
{
    UpdateDMG(time);
    CalculateModelMatrix(time);
    UpdateLight(time);
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
    cur_angle_Z += rot_angle_Z * time;
    ZC_Mat4<float> model = ZC_Mat4<float>(1.f).Rotate(cur_angle_Z, {0.f, 0.f, 1.f}).Translate({ 0.f, dist_to_center, 10.f });
    ZC_Vec3<float> star_pos_cur = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, 0.f, 0.f, 1.f));

        //  update pos in collision, particles, and light
    collision_object.UpdateModelMatrix(model);
    ps_star.Set_SpawnMatModel__translation(star_pos_cur);
    
    static const uint color_packed = ZC_Pack_Float_To_UInt_2x10x10x10(1.f, 0.86f, 0.55f);    //  peach color
    G_LightUBO::UpdateLightSet(G_LN__Star, G_LightSet{ .pos = star_pos_cur, .color = color_packed });
}

void G_Star::UpdateLight(float time)
{
    light_secs_to_move -= time;
    if (light_secs_to_move <= 0) RecalcLightData();
        //  attenuation
    G_LightUBO::UpdateAttenuation(G_LAN_PlayerSphere,
        G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_PlayerSphere].linear + (light_linear_speed_player_sphere * time), .quadratic = G_LightUBO::c_light.attenuations[G_LAN_PlayerSphere].quadratic });
    G_LightUBO::UpdateAttenuation(G_LAN_Platform,
        G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_Platform].linear + (light_linear_speed_platform * time), .quadratic = G_LightUBO::c_light.attenuations[G_LAN_Platform].quadratic });
    G_LightUBO::UpdateAttenuation(G_LAN_CubeMap,
        G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear + (light_linear_speed_cube_map * time), .quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic });
        //  color                                       sets in CalculateModelMatrix()
    G_LightUBO::UpdateLightSet(G_LN__Star, G_LightSet{ .pos = ps_star.c_ps_src.spawn_mat_model.translate, .color = ZC_Pack_Float_To_UInt_2x10x10x10(light_color_cur += light_color_move_speed * time) });
}

void G_Star::RecalcLightData()
{
    light_secs_to_move = ZC_Random::GetRandomFloat_x_100(1.f, 10.f) / 10.f;
        //  attenuation. Move to random attenuation near light_linear_start_... data 
    float offset_percent = ZC_Random::GetRandomFloat_x_100(5.f, 70.f) / 100.f;
    float move_signe = light_linear_start_player_sphere < G_LightUBO::c_light.attenuations[G_LAN_PlayerSphere].linear ? -1.f : 1.f;

    auto lamb_calc_attenation_speed = [this, offset_percent, move_signe](float val_start, float val_cur) -> float
    {
        float dest = val_start + (val_start * offset_percent) * move_signe;
        float move_length = move_signe > 0 ? dest - val_cur : val_cur - dest;
        return move_length / light_secs_to_move * move_signe;
    };
    light_linear_speed_player_sphere = lamb_calc_attenation_speed(light_linear_start_player_sphere, G_LightUBO::c_light.attenuations[G_LAN_PlayerSphere].linear);
    light_linear_speed_platform = lamb_calc_attenation_speed(light_linear_start_platform, G_LightUBO::c_light.attenuations[G_LAN_Platform].linear);
    light_linear_speed_cube_map = lamb_calc_attenation_speed(light_linear_start_cube_map, G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear);

        //  color
    auto lamb_calc_color_speed = [this](float start, float end) -> float
    {       //  start > end, speed must be negative
        return (start > end ? - (start - end) : end - start) / light_secs_to_move;
    };
    ZC_Vec3<float> color_peach(1.f, 0.86f, 0.55f);
    light_color_move_speed = ZC_Vec3<float>        //  random destination color
        (
            0,
            lamb_calc_color_speed(light_color_cur[1], color_peach[1] + (color_peach[1] * (ZC_Random::GetRandomFloat_x_100(-1, 1) / 10.f))),
            lamb_calc_color_speed(light_color_cur[2], color_peach[2] + (color_peach[2] * (ZC_Random::GetRandomFloat_x_100(-1, 1) / 10.f)))
        );
}

#else

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
    static const float speed_horizontal_move = 8.f;     //  speed of movement around the map circle

    float new_length = 2.f * ZC_PI * dist_to_star;
    rot_angle_Z = ZC_angle_360f / (new_length / speed_horizontal_move);

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
    ss_star.SetPosition(&temp_pos);     //  origin in collision object at first game will be (0,0,0), collision not happend yet to move us. So need set temp pos somwhere to make sound set connected to cam pos updater but without cam distance volume 0
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
    UpdateDMG(time);
    CalculateModelMatrix(time);
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

    cur_angle_XY += rot_speed_XY * time;
    cur_angle_Z += rot_angle_Z * time;
    ZC_Mat4<float> model = ZC_Mat4<float>(1.f).Rotate(cur_angle_Z, {0.f, 0.f, 1.f}).Translate({ 0.f, dist_to_center, 10.f })
        .Rotate(cur_angle_XY, { 1.f, 1.f, 0.f }).Scale(star_scale, star_scale, star_scale);
    ZC_Vec3<float> star_center = ZC_Vec::Vec4_to_Vec3(model * ZC_Vec4<float>(0.f, 0.f, 0.f, 1.f));

        //  update pos in collision, particles, and light
    collision_object.UpdateModelMatrix(model);
    particles_star.SetPosition(star_center);
    static const uint color_packed = ZC_Pack_Float_To_UInt_2x10x10x10(1.f, 0.86f, 0.55f);    //  peach color
    G_LightUBO::UpdateLightSet(G_LN__Star, G_LightSet{ .pos = star_center, .color = color_packed });
}
#endif