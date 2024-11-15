#include "G_Map.h"

G_Map::G_Map()
    : start_platform(G_PlatformTransforms{ .translate = platforms_start_pos, .scale = { scaleXY_start_platform, scaleXY_start_platform, scaleZ_platform } })
{
        //  calculate positions of the platforms on the lines
    float other_platforms_diameter = (platform_model_radius_XY * 2.f) * scaleXY_other_platforms;
    dist_to_first_platform = (platform_model_radius_XY * scaleXY_start_platform) + distance_between_platforms + (other_platforms_diameter / 2.f);   //  first platform radious + distance + other platfrom rafius
    dist_between_other_platforms = other_platforms_diameter + distance_between_platforms;
    half_dist_between_other_platforms = dist_between_other_platforms / 2.f;
        //  it is equal to the distance of the platform position from the first circle
    first_section_lenght = dist_to_first_platform + (dist_between_other_platforms * platforms_on_line);
    other_section_length = dist_between_other_platforms * (platforms_on_line + 1);    //  distance from circle of previous section to circle of current section (1 is platform from circle)
}

void G_Map::CreateLevel(int _lvl)
{
    sections.clear();
        //  calculate sections and lines count
    int lvl = _lvl;
    int sections_count = std::ceil(float(lvl) / float(lines_count_in_first_level));
    
    int minus_lines_on_level = (lvl % lines_count_in_first_level);
    int lines_count = minus_lines_on_level != 0 ? lines_count_in_first_level - (minus_lines_on_level - 1) : 1;
    
    dist_to_last_platform_center = first_section_lenght + (other_section_length * (sections_count - 1));    //  must be calculated before damage platfrom creation. Look G_PlatfromDamage::G_PlatfromDamage()
        
        //  fill sections
    int next_section_lines_count = lines_count;
    for (int section_i = 0; section_i < sections_count; ++section_i, next_section_lines_count *= 2)
    {       //  distacne to frist platorm on line in section
        float dist_to_first_platform_in_section = section_i == 0 ? dist_to_first_platform     //  most first platform after start (origin) platform
            : (first_section_lenght + dist_between_other_platforms + (other_section_length * (section_i - 1)));
        float distance_to_circle_platform = first_section_lenght + (other_section_length * section_i);  //  radius of the circle

        sections.emplace_back(G_Section(next_section_lines_count, platforms_on_line, dist_to_first_platform_in_section, distance_to_circle_platform,
            section_i == sections_count - 1));
    }


    float dist_to_star = dist_to_last_platform_center + dist_between_other_platforms * 1.5f;
    star.SetNewPosition(dist_to_star);
    star.VSetDefaultState_IO();

    static const float dist_star_center_to_map_sphere = dist_between_other_platforms * 3.f;
    map_sphere.SetScale(dist_to_star + dist_star_center_to_map_sphere);    //  set map shpere radius 
}

float G_Map::GetMapSphereScale() const noexcept
{
    return map_sphere.GetScale();
}