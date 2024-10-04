#include "G_GUI_GM_W_LevelTimer.h"

#include <ZC/GUI/ZC__GUI.h>

G_GUI_GM_W_LevelTimer::G_GUI_GM_W_LevelTimer()
    : ZC_GUI__WinImmutable(ZC_WOIData(
        ZC__GUI::GetLongestNumberCharacterWidth() * 6 + ZC__GUI::CalculateWstrWidth(L":") * 2 + ZC__GUI::GetLongestNumberCharacterWidth() / 5.f * 5.f,  //  calc window width ( (* 5.f) is 5 distances loock AddRow())
        ZC__GUI::GetFontHeight(), 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Stacionar | ZC_GUI_WF__NoBackground),
    text_hours(L"00", false, ZC__GUI::GetLongestNumberCharacterWidth() * 2, ZC_GUI_TextAlignment::Center),
    text_minute(L"00", false, ZC__GUI::GetLongestNumberCharacterWidth() * 2, ZC_GUI_TextAlignment::Center),
    text_seconds(L"00", false, ZC__GUI::GetLongestNumberCharacterWidth() * 2, ZC_GUI_TextAlignment::Center),
    text_colon_1(L":", true, 0, ZC_GUI_TextAlignment::Left),
    text_colon_2(L":", true, 0, ZC_GUI_TextAlignment::Left)
{
    this->AddRow(ZC_GUI_Row(ZC_GUI_Row(ZC_GUI_RowParams(0, ZC_GUI_RowParams::Left, 0, ZC__GUI::GetLongestNumberCharacterWidth() / 5),
        { text_hours.GetObj(), text_colon_1.GetObj(), text_minute.GetObj(), text_colon_2.GetObj(), text_seconds.GetObj() })));
}

void G_GUI_GM_W_LevelTimer::UpdateHours(const std::wstring& text)
{
    text_hours.UpdateText(text, true);
}

void G_GUI_GM_W_LevelTimer::UpdateMinutes(const std::wstring& text)
{
    text_minute.UpdateText(text, true);
}

void G_GUI_GM_W_LevelTimer::UpdateSeconds(const std::wstring& text)
{
    text_seconds.UpdateText(text, true);
}