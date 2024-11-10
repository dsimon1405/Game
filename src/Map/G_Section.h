#pragma once

#include <Objects/Platforms/G_Platform.h>
#include <ZC/Events/ZC_EC.h>

#include <vector>

class G_Section
{
public:
    G_Section(int lines_count, int platforms_on_line, float dist_to_first_platform_in_section, float distance_to_circle_platform, bool is_last_section);
    G_Section(G_Section&& s);
    ~G_Section();

private:
    std::vector<ZC_uptr<G_Platform>> platforms_on_lines;
    std::vector<ZC_uptr<G_Platform>> platforms_on_circle;
    ZC_EC ecUpdater;

    struct RotateSet
    {
        float rotate_angle = 0.f;   //  random angle to ratate on at first platforms at first internal, then external
        float cur_rotate_angle = 0.f;
        bool is_internal_rotation = true;   //  if not internal then external rotation
    };

    RotateSet rotSet_lines;
    RotateSet rotSet_circle;
    bool last_section;

    float rotation_angle_external_lines = 0.f;
    float rotation_angle_external_circle = 0.f;

    static inline G_Platform* pPlat_win = nullptr;

    static ZC_uptr<G_Platform> GetRandomPlatform(const ZC_Vec3<float>& translate);

    void FillPlatforms(int lines_count, int platforms_on_line, float dist_to_first_platform_in_section, float distance_to_circle_platform, bool is_last_section);
    
    void Callback_Updater(float time);
    void RotatePlatforms(RotateSet& rotate_set, float time, std::vector<ZC_uptr<G_Platform>>& platforms, float rotation_speed_external);

        //  find new place for win platform on the outer circle
    void Callback_SwitchWinPlatfrom(G_Platform* pPlat_win);
};