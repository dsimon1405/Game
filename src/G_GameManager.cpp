#include "G_GameManager.h"

#include <Model/G_Models.h>
#include <Objects/G_Player.h>
#include <Objects/G_Platform.h>

G_GameaManager::G_GameaManager()
{
    G_Models::LoadModels();
    
    objs.emplace_back(new G_Player());
 
 
        //  platform params
    const float platforms_model_radius = 3.f;      //  loaded model radius radius
    const float scaleXY_first_platform = 4.f;
    const float scaleXY_other_platforms = 2.5f;
    const float scaleZ_platform = 2.5f;
    const float distance_between_platforms = 10.f;    //  distance between platform on lines and on circles
    const ZC_Vec3<float> platforms_start_pos(0.f, 0.f, -1.f);
        //  section data
    int sections_count = 3;     //  section is platforms on line limited with circle 
    const int lines_count = 6;
    const int platforms_on_line = 3;    //  platforms between cirles or start platdorm
        //  calculate lines dirs
    std::vector<ZC_Vec3<float>> lines_directions;
    lines_directions.reserve(lines_count);
    lines_directions.emplace_back(ZC_Vec3<float>(0.f, 1.f, 0.f));   //  first direction
    const float angle_between_lines = 360.f / lines_count;
    ZC_Vec4<float> first_line_dir(lines_directions[0], 1.f);
    for (float angle = angle_between_lines; angle < 360.f; angle += angle_between_lines)    //  calculate dirs for other lines
        lines_directions.emplace_back(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle, { 0.f, 0.f, 1.f }) * first_line_dir));
        //  add start platform
    ZC_Vec3<float> first_platform_scale(scaleXY_first_platform, scaleXY_first_platform, scaleZ_platform);
    objs.emplace_back(new G_Platform(ZC_Mat4<float>(1.f).Translate(platforms_start_pos).Scale(first_platform_scale), first_platform_scale));
        //  calculate positions of the platforms on the lines
    float other_platforms_diameter = (platforms_model_radius * 2.f) * scaleXY_other_platforms;
    float dist_to_first_platform = (platforms_model_radius * scaleXY_first_platform) + distance_between_platforms + (other_platforms_diameter / 2.f);   //  first platform radious + distance + other platfrom rafius
    float dist_between_other_platforms = other_platforms_diameter + distance_between_platforms;
    float half_dist_between_other_platforms = dist_between_other_platforms / 2.f;
        //  it is equal to the distance of the platform position from the first circle
    float first_section_lenght = dist_to_first_platform + (dist_between_other_platforms * platforms_on_line);
    float other_section_length = dist_between_other_platforms * (platforms_on_line + 1);    //  distance from circle of previous section to circle of current section (1 is platform from circle)
    ZC_Vec3<float> other_platform_scale(scaleXY_other_platforms, scaleXY_other_platforms, scaleZ_platform);
        //  add platforms
    for (int section_i = 0; section_i < sections_count; ++section_i)
    {       //  distacne to frist platorm on line in section
        float dist_to_first_platform_in_section = section_i == 0 ? dist_to_first_platform     //  most first platform after start (origin) platform
            : (first_section_lenght + dist_between_other_platforms + (other_section_length * (section_i - 1)));
        for (int line_i = 0; line_i < lines_count; ++line_i)    //  add platfroms on lines
        {
            for (int platform_i = 0; platform_i < platforms_on_line; ++platform_i)
            {
                objs.emplace_back(new G_Platform(ZC_Mat4<float>(1.f)
                    .Translate(ZC_Vec::MoveByLength(platforms_start_pos, lines_directions[line_i], dist_to_first_platform_in_section + (dist_between_other_platforms * platform_i)))
                    .Scale(other_platform_scale), other_platform_scale));
            }
        }

            //  add platforms in circle
        float distance_to_circle_platform = first_section_lenght + (other_section_length * section_i);  //  radius of the circle
            //  in isoscelec triangle (A = {0,0,0}, B and C two nearby platforms) dist_between_other_platforms is triangle base and distance_to_circle_platform is sides
            //  half_dist_between_other_platforms is leg of right triangle (inside isosceles triangle) for calculation half angle opposite to base (dist_between_other_platforms) were distance_to_circle_platform is hypotenuse
        float angle_between_platforms = std::ceil(ZC_Vec::Degrees(std::asin(half_dist_between_other_platforms / distance_to_circle_platform) * 2.f));
        for (float angle = 0; angle <= 360.f; angle += angle_between_platforms)    //  calculate dirs for other lines
        {
            ZC_Vec3<float> dir = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle, { 0.f, 0.f, 1.f }) * first_line_dir);
            objs.emplace_back(new G_Platform(ZC_Mat4<float>(1.f)
                .Translate(ZC_Vec::MoveByLength(platforms_start_pos, dir, distance_to_circle_platform))
                .Scale(other_platform_scale), other_platform_scale));
        }
    }
    

 
    // float cylinder_radius = 3.f;
    // float scale = 2.5f;
    // float trans = cylinder_radius * scale * 2.f + 10.f;
    // ZC_Vec3<float> platform_scale(scale, scale, 3.f);
    // objs.emplace_back(new G_Platform(ZC_Mat4<float>(1.f).Translate({0.f, 0.f, -1.f}).Scale(platform_scale), platform_scale));
    // objs.emplace_back(new G_Platform(ZC_Mat4<float>(1.f).Translate({trans, 0.f, -1.f}).Scale(platform_scale), platform_scale));



    for (auto& upObj : objs) upObj->SetEventAndUpdateState(true);
}