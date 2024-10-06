#include "G_GUI_W_ConfigSaveLevel.h"

#include <ZC/Video/ZC_SWindow.h>
#include <ZC/GUI/ZC__GUI.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/GUI/Backend/Config/ZC_GUI_Colors.h>

G_GUI_W_ConfigSaveLevel::G_GUI_W_ConfigSaveLevel()
    : text_Level_saved(ZC_GUI_TextAlignment::Left, ZC_GUI_Colors::window_text, 
        { G_LangText{ .lang = G_L_Russian, .text = L"ур. сохранён" }, { .lang = G_L_English, .text = L"level saved" } }),
    text_level(L"", false, ZC__GUI::GetLongestNumberCharacterWidth() * 4, ZC_GUI_TextAlignment::Right),
    text_time(L"", false, ZC__GUI::GetLongestNumberCharacterWidth() * 6 + ZC__GUI::CalculateWstrWidth(L":") * 2, ZC_GUI_TextAlignment::Center),
    window(ZC_WOIData(text_level.GetWidth() + text_Level_saved.GetWidth() + ZC__GUI::GetFontHeight() / 2.f,
        ZC__GUI::GetFontHeight() * 2.f, ZC__GUI::GetFontHeight() / 3.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Bottom_Pixel), ZC_GUI_WF__Stacionar | ZC_GUI_WF__NoBackground
        // | ZC_GUI_WF__NeedDraw)
        )
{
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 0, ZC__GUI::GetFontHeight() / 2.f), { text_Level_saved.GetObj(), text_level.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(ZC__GUI::GetFontHeight() * 1.5f, ZC_GUI_RowParams::Right, 0, 0), { text_time.GetObj() }));
}

G_GUI_W_ConfigSaveLevel::~G_GUI_W_ConfigSaveLevel()
{
    ecUpdater.Disconnect();
}

void G_GUI_W_ConfigSaveLevel::ShowSavedData(int level, G_Time time)
{
    text_level.UpdateText(std::to_wstring(level), true);
    text_time.UpdateText(
            (time.hours > 9 ? std::to_wstring(time.hours) : L"0" + std::to_wstring(time.hours)) + L":"
            + (time.minutes > 9 ? std::to_wstring(time.minutes) : L"0" + std::to_wstring(time.minutes)) + L":"
            + (time.seconds > 9 ? std::to_wstring(time.seconds) : L"0" + std::to_wstring(time.seconds)),
        true);

    cur_time = 0.f;
    wait_to_show = true;
    ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_GUI_W_ConfigSaveLevel::Callback_Updater, this }, G_UpdaterLevels::G_UL__w_player_win));
}

void G_GUI_W_ConfigSaveLevel::Callback_Updater(float time)
{
    static const float wait_seconds = 3.f;
    static const float show_seconds = 4.f;

    cur_time += time;
    
    if (wait_to_show)
    {
        if (cur_time >= wait_seconds)
        {
            wait_to_show = false;
            cur_time = 0.f;
            window.SetDrawState(true);
        }
    }
    else
    {
        if (cur_time >= show_seconds)
        {
            window.SetDrawState(false);
            ecUpdater.Disconnect();
            cur_time = 0.f;
        }
    }
}