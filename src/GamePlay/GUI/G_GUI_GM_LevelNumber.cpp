#include "G_GUI_GM_LevelNumber.h"

G_GUI_GM_LevelNumber::G_GUI_GM_LevelNumber()
    : tstc_level_start_scale(tstc_level.GetScale())
{}

void G_GUI_GM_LevelNumber::SetDefaultState(int level)
{
    cur_time = 0.f;
    tstc_level.UpdateLevelText(level);
    tstc_level.SetPosition({ 0.f, 0.f, tstc_level_start_Z });
    tstc_level.SetScale(tstc_level_start_scale);
    tstc_level.SetColorUChar(130, 130, 130);
}

bool G_GUI_GM_LevelNumber::Update(float time)
{
    cur_time += time;

    //  when game started (look lower) make scale and move up - level text
    static const float seconds_txts_prepare = 2.f;
    static const float tstc_level_length_to_end_Z = 10.f - tstc_level_start_Z;      //  10 is end
    static const float tstc_level_range_to_end_scale = 0.05f - tstc_level_start_scale;  //  0.05 is end
    
    if (cur_time >= seconds_txts_prepare) return true;
    
    float time_coef = cur_time / seconds_txts_prepare;
    tstc_level.SetPosition({ 0.f, 0.f, tstc_level_start_Z + (tstc_level_length_to_end_Z * time_coef) });
    tstc_level.SetScale(tstc_level_start_scale + (tstc_level_range_to_end_scale * time_coef));
    return false;
}