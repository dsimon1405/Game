#pragma once

#include <GUI/Text/G_ZC_TextSceneTurnedToCameraLevel.h>

class G_GUI_GM_LevelNumber
{
public:
    G_GUI_GM_LevelNumber();

    void SetDefaultState(int level);
        //  move level number up and scale. If in proccess return false, otherwise true
    bool Update(float time);

private:
    float cur_time = 0.f;
    G_ZC_TextSceneTurnedToCameraLevel tstc_level;
    const float tstc_level_start_Z = 6.f;
    const float tstc_level_start_scale;
};