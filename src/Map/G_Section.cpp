#include "G_Section.h"

#include "G_Map.h"
#include <Objects/Platforms/G_PlatformDisappear.h>
#include <Objects/Platforms/G_PlatformWin.h>
#include <Objects/Platforms/G_PlatformScale.h>
#include <Objects/Platforms/G_PlatformWind.h>
#include <Objects/Platforms/G_PlatformDamage.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>

G_Section::G_Section(int lines_count, int platforms_on_line, float dist_to_first_platform_in_section, float distance_to_circle_platform, bool is_last_section)
    : ecUpdater(ZC__Updater::Connect({ &G_Section::Callback_Updater, this }, G_UL__game_play)),
    rotSet_lines{ .rotate_angle = float(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i)) },
    rotSet_circle{ .rotate_angle = float(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i)) },
    last_section(is_last_section)
{
    //     //  calculation of the rotation speeds (example to rememner). Calculation and speed correct for sections with 3 platforms at the line.
    // const float rotation_angle_external_lines = 5.f;      //  5 degrees per second
    // const float rotation_angle_external_circle = 1.f;      //  5 degrees per second
    // const float dist_to_last_platfrom = 104.5f;   //  dist to last platfrom of the first section
    // const float dist_between_other_platforms = 25.f;     //  other platfroms is all platfrom among first (latge) platform
    // float last_platform_at_line_circle_length = 2.f * ZC_PI * (dist_to_last_platfrom - dist_between_other_platforms);     //  length of the circle formed by last platform of the line
    // float speed_dist_rotation_for_lines = last_platform_at_line_circle_length / (360.f / rotation_angle_external_lines); //  6.94
    // float circle_length = 2.f * ZC_PI * dist_to_last_platfrom;     //  length of the circle 
    // float rot_lenght_lines = circle_length / (360.f / rotation_angle_external_circle);   //  1.82
    
        //  Speed correct for sections with 3 platforms at the line.
    static const float speed_dist_rotation_for_lines = 6.94f;   //  length / second -> external rotation speed for platforms of the lines
    static const float speed_dist_rotation_for_circle = 1.82f;  //  lenfth / second -> external rotation speed for platfroms of the circle
    
    float last_platform_at_line_circle_length = 2.f * ZC_PI * (distance_to_circle_platform - G_Map::dist_between_other_platforms);
    rotation_angle_external_lines = ZC_angle_360f / (last_platform_at_line_circle_length / speed_dist_rotation_for_lines);
    float circle_length = 2.f * ZC_PI * distance_to_circle_platform;
    rotation_angle_external_circle = ZC_angle_360f / (circle_length / speed_dist_rotation_for_circle);

    FillPlatforms(lines_count, platforms_on_line, dist_to_first_platform_in_section, distance_to_circle_platform, is_last_section);
}

G_Section::G_Section(G_Section&& s)
    : platforms_on_lines(std::move(s.platforms_on_lines)),
    platforms_on_circle(std::move(s.platforms_on_circle)),
    ecUpdater(ZC__Updater::Connect({ &G_Section::Callback_Updater, this }, G_UL__game_play)),
    rotSet_lines(s.rotSet_lines),
    rotSet_circle(s.rotSet_circle),
    rotation_angle_external_lines(s.rotation_angle_external_lines),
    rotation_angle_external_circle(s.rotation_angle_external_circle)
{
    s.ecUpdater.Disconnect();
    if (pPlat_win) dynamic_cast<G_PlatformWin*>(pPlat_win)->Update_func_change_pos({ &G_Section::Callback_SwitchWinPlatfrom, this });
}

G_Section::~G_Section()
{
    ecUpdater.Disconnect();
    if (last_section) pPlat_win = nullptr;
}

ZC_uptr<G_Platform> G_Section::GetRandomPlatform(const ZC_Vec3<float>& translate)
{
    // return new G_PlatformDamage(G_PlatformTransforms{ .translate = translate, .scale = G_Map::other_platform_scale });
    switch (ZC_Random::GetRandomInt(G_PT__Damage, G_PT__Wind))
    {
    case G_PT__Damage: return new G_PlatformDamage(G_PlatformTransforms{ .translate = translate, .scale = G_Map::other_platform_scale });
    case G_PT__Disapear: return new G_PlatformDisappear(G_PlatformTransforms{ .translate = translate, .scale = G_Map::other_platform_scale });
    case G_PT__Scale: return new G_PlatformScale(G_PlatformTransforms{ .translate = translate, .scale = G_Map::other_platform_scale });
    case G_PT__Wind: return new G_PlatformWind(G_PlatformTransforms{ .translate = translate, .scale = G_Map::other_platform_scale });
    default: assert(false); return nullptr;
    }
}

void G_Section::FillPlatforms(int lines_count, int platforms_on_line, float dist_to_first_platform_in_section, float distance_to_circle_platform, bool is_last_section)
{
        //  calculate lines dirs
    std::vector<ZC_Vec3<float>> lines_directions;
    lines_directions.reserve(lines_count);
    lines_directions.emplace_back(ZC_Vec3<float>(0.f, 1.f, 0.f));   //  first direction
    const float angle_between_lines = ZC_angle_360f / lines_count;
    ZC_Vec4<float> first_line_dir(lines_directions[0], 1.f);
    for (float angle = angle_between_lines; angle < ZC_angle_360f; angle += angle_between_lines)    //  calculate dirs for other lines
        lines_directions.emplace_back(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle, { 0.f, 0.f, 1.f }) * first_line_dir));
    
        //  add platfroms on lines
    platforms_on_lines.reserve(lines_count * platforms_on_line);
    for (int line_i = 0; line_i < lines_count; ++line_i)
    {
        for (int platform_i = 0; platform_i < platforms_on_line; ++platform_i)
        {
            platforms_on_lines.emplace_back(GetRandomPlatform(ZC_Vec::MoveByLength(G_Map::platforms_start_pos, lines_directions[line_i],
                    dist_to_first_platform_in_section + (G_Map::dist_between_other_platforms * platform_i))));
        }
    }

        //  in isoscelec triangle (A = {0,0,0}, B and C two nearby platforms) dist_between_other_platforms is triangle base and distance_to_circle_platform is sides
        //  half_dist_between_other_platforms is leg of right triangle (inside isosceles triangle) for calculation half angle opposite to base (dist_between_other_platforms) were distance_to_circle_platform is hypotenuse
    float angle_between_platforms = ZC_Vec::Degrees(std::asin(G_Map::half_dist_between_other_platforms / distance_to_circle_platform) * 2.f);
    int platforms_on_circle_count = std::ceil(ZC_angle_360f / angle_between_platforms);
    angle_between_platforms = ZC_angle_360f / float(platforms_on_circle_count);
    
    platforms_on_circle.reserve(platforms_on_circle_count);
    int win_platform_id = is_last_section ? ZC_Random::GetRandomInt(0, platforms_on_circle_count - 1) : -1;
    float cur_angle = 0;
    for (int platform_i = 0; platform_i < platforms_on_circle_count; ++platform_i, cur_angle += angle_between_platforms)    //  calculate dirs for other lines
    {
        ZC_Vec3<float> dir = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(cur_angle, { 0.f, 0.f, 1.f }) * first_line_dir);
        if (platform_i == win_platform_id) pPlat_win = platforms_on_circle.emplace_back(new G_PlatformWin(
            G_PlatformTransforms{ .translate = ZC_Vec::MoveByLength(G_Map::platforms_start_pos, dir, distance_to_circle_platform), .scale = G_Map::other_platform_scale },
            { &G_Section::Callback_SwitchWinPlatfrom, this })).Get();
        else platforms_on_circle.emplace_back(GetRandomPlatform(ZC_Vec::MoveByLength(G_Map::platforms_start_pos, dir, distance_to_circle_platform)));
    }
}

void G_Section::Callback_Updater(float time)
{

    RotatePlatforms(rotSet_lines, time, platforms_on_lines, rotation_angle_external_lines);
    RotatePlatforms(rotSet_circle, time, platforms_on_circle, rotation_angle_external_circle);
}

void G_Section::RotatePlatforms(RotateSet& rotate_set, float time, std::vector<ZC_uptr<G_Platform>>& platforms, float rotation_speed_external)
{
    static const float rotation_speed_internal = 25.f;      //  25 degrees per second

    float angle = (rotate_set.is_internal_rotation ? rotation_speed_internal : rotation_speed_external) * time;
    if (rotate_set.rotate_angle < 0.f) angle *= -1.f;
    rotate_set.cur_rotate_angle += angle;
    for (ZC_uptr<G_Platform>& upPlatf : platforms) rotate_set.is_internal_rotation ? upPlatf->RotateInternal(angle) : upPlatf->RotateExternal(angle);
        //  check cur_rotate_angle limit, if overflow, randomize new angle. If rotation angle negative need check low border, otherwise top border
    if (rotate_set.rotate_angle >= 0.f ? rotate_set.cur_rotate_angle >= rotate_set.rotate_angle : rotate_set.cur_rotate_angle <= rotate_set.rotate_angle)     //  change rotation
    {
        rotate_set.is_internal_rotation = !rotate_set.is_internal_rotation;
        rotate_set.cur_rotate_angle = 0.f;
        if (rotate_set.is_internal_rotation) rotate_set.rotate_angle = ZC_Random::GetRandomInt(-ZC_angle_360i, ZC_angle_360i);  //  change angle for next internal-external phase
    }
}

void G_Section::Callback_SwitchWinPlatfrom(G_Platform* pPlat_win)
{
    int new_pos_id = ZC_Random::GetRandomInt(0, platforms_on_circle.size() - 1);
    ZC_uptr<G_Platform>& pPlat = platforms_on_circle[new_pos_id];
    if (pPlat.Get() != pPlat_win && pPlat->SwitchWithWinPlatform(pPlat_win)) return;    //  found platform is not win platform and successfull switched
    else Callback_SwitchWinPlatfrom(pPlat_win);    //  something go wrong, try again
}