#include "G_GUI_GM_W_LevelTimer.h"

#include <ZC/GUI/ZC__GUI.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <GUI/Text/G_GUI_Fonts.h>

#include <cmath>

G_GUI_GM_W_LevelTimer::G_GUI_GM_W_LevelTimer()
    : ZC_GUI__WinImmutable(ZC_WOIData(
        G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 6 + G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->CalculateWstrWidth(L":") * 2
        + G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width / 5.f * 5.f,  //  calc window width ( (* 5.f) is 5 distances loock AddRow())
        G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40), 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Stacionar | ZC_GUI_WF__NoBackground),
    text_hours(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"00", false, G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 2,
        ZC_GUI_TextAlignment::Center, ZC_Pack_UChar_To_UInt_2x10x10x10(150, 150, 150)),
    text_minute(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"00", false, G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 2,
        ZC_GUI_TextAlignment::Center, ZC_Pack_UChar_To_UInt_2x10x10x10(150, 150, 150)),
    text_seconds(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"00", false, G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 2,
        ZC_GUI_TextAlignment::Center, ZC_Pack_UChar_To_UInt_2x10x10x10(150, 150, 150)),
    text_colon_1(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L":", true, 0, ZC_GUI_TextAlignment::Left, ZC_Pack_UChar_To_UInt_2x10x10x10(150, 150, 150)),
    text_colon_2(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L":", true, 0, ZC_GUI_TextAlignment::Left, ZC_Pack_UChar_To_UInt_2x10x10x10(150, 150, 150))
{
    this->AddRow(ZC_GUI_Row(ZC_GUI_Row(ZC_GUI_RowParams(0, ZC_GUI_RowParams::X_Left, 0, G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width / 5, ZC_GUI_RowParams::Y_Center),
        { text_hours.GetObj(), text_colon_1.GetObj(), text_minute.GetObj(), text_colon_2.GetObj(), text_seconds.GetObj() })));
}

void G_GUI_GM_W_LevelTimer::Start()
{
    ecUpdater.NewConnection(ZC__Updater::Connect({ &G_GUI_GM_W_LevelTimer::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play));
}

void G_GUI_GM_W_LevelTimer::SetDefaultState()
{
        //  text time
    if (time_cur_lvl.hours != 0) text_hours.UpdateText(L"00", true);
    if (time_cur_lvl.minutes != 0) text_minute.UpdateText(L"00", true);
    if (time_cur_lvl.seconds != 0) text_seconds.UpdateText(L"00", true);
        //  timer
    time_cur_lvl = {};

    ecUpdater.Disconnect();
}

G_Time G_GUI_GM_W_LevelTimer::GetTime() const noexcept
{
    return time_cur_lvl;
}

void G_GUI_GM_W_LevelTimer::Callback_Updater(float time)
{
    static const float one_second = 1.f;
        
    cur_time += time;
    if (cur_time >= one_second)
    {
        G_Time prev_time = time_cur_lvl;

        float seconds = 0.f;    //  whole part
        cur_time = std::modf(cur_time, &seconds);
        time_cur_lvl.PlusSeconds(seconds);    //  whole part of the time (seconds) add to time_cur_lvl, fractional set in cur_time
            //  update text
        if (prev_time.hours != time_cur_lvl.hours)
            text_hours.UpdateText(time_cur_lvl.hours > 9 ? std::to_wstring(time_cur_lvl.hours) : L"0" + std::to_wstring(time_cur_lvl.hours), true);
        if (prev_time.minutes != time_cur_lvl.minutes)
            text_minute.UpdateText(time_cur_lvl.minutes > 9 ? std::to_wstring(time_cur_lvl.minutes) : L"0" + std::to_wstring(time_cur_lvl.minutes), true);
        if (prev_time.seconds != time_cur_lvl.seconds)
            text_seconds.UpdateText(time_cur_lvl.seconds > 9 ? std::to_wstring(time_cur_lvl.seconds) : L"0" + std::to_wstring(time_cur_lvl.seconds), true);
    }
}