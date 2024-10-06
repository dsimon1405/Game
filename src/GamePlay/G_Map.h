#pragma once

#include "G_Section.h"

#include <list>

class G_Map
{
public:
    static inline const float scaleXY_other_platforms = 2.5f;
    static inline const float scaleZ_platform = 2.5f;
    static inline const float platforms_model_radius = 3.f;      //  loaded model radius radius
    static inline const float scaleXY_start_platform = 4.f;
    static inline const float distance_between_platforms = 10.f;    //  distance between platform on lines and on circles
    static inline const ZC_Vec3<float> platforms_start_pos { 0.f, 0.f, -1.f };
    static inline ZC_Vec3<float> other_platform_scale { scaleXY_other_platforms, scaleXY_other_platforms, scaleZ_platform };

        //  calculated
    static inline float dist_to_first_platform = 0.f;
    static inline float dist_between_other_platforms = 0.f;
    static inline float half_dist_between_other_platforms = 0.f;
    static inline float first_section_lenght = 0.f;
    static inline float other_section_length = 0.f;
    static inline float map_radius = 0.f;   //  distance to center of platform from last cyrcle

    G_Map();

    void CreateLevel(int _lvl);

private:
    const int lines_count_in_first_level = 6;
    const int platforms_on_line = 3;    //  platforms between on line in section
    
    G_Platform start_platform;
    std::list<G_Section> sections;
};