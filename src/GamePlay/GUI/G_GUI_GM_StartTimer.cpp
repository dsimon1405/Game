#include "G_GUI_GM_StartTimer.h"

#include <System/G_FontData.h>

G_GUI_GM_StartTimer::G_GUI_GM_StartTimer()
    : tw_start_counter
        {
            ZC_TextWindow(G_FontData::fd_Arial_200, L"3", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, L"2", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, L"1", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false)
        },
    sound_timer(G_SN__gui_start_timer)
{
    tw_start_counter[0].SetColorUChar(255, 0, 0);
    tw_start_counter[1].SetColorUChar(255, 255, 0);
    tw_start_counter[2].SetColorUChar(0, 255, 0);
}


void G_GUI_GM_StartTimer::SetDefaultState()
{
    cur_time = 0.f;
    tw_index = 0;
        //  enable first, other off
    tw_start_counter[0].NeedDraw(true);
    tw_start_counter[1].NeedDraw(false);
    tw_start_counter[2].NeedDraw(false);
        //  sound
    sound_timer.SetSoundState(ZC_SS__Stop);
}

bool G_GUI_GM_StartTimer::Update(float time)
{
    if (tw_index == 3) return true;

    if (cur_time == 0.f) sound_timer.SetSoundState(ZC_SS__Play);

    cur_time += time;
        //  show counter one by one, then start level
    static const float start_second = 1.f;
    if (cur_time >= start_second)
    {
        tw_start_counter[tw_index].NeedDraw(false);
        cur_time = 0.f;
        ++tw_index;
        if (tw_index == 3) return true;
        else tw_start_counter[tw_index].NeedDraw(true);
    }
    tw_start_counter[tw_index].SetAlpha(1.f - (cur_time / start_second));
    return false;
}

bool G_GUI_GM_StartTimer::IsFinished()
{
    return tw_index == 3;
}