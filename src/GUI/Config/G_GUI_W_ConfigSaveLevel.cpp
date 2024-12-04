#include "G_GUI_W_ConfigSaveLevel.h"

#include <ZC/ZC__System.h>
#include <ZC/GUI/ZC__GUI.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/GUI/Backend/Config/ZC_GUI_Colors.h>
#include <GUI/Text/G_GUI_Fonts.h>

G_GUI_W_ConfigSaveLevel::G_GUI_W_ConfigSaveLevel()
    : text_Level_saved(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Left, ZC_GUI_Colors::window_text,
        { G_LangText{ .lang = G_L_Russian, .text = L"ур. сохранён" }, { .lang = G_L_English, .text = L"level saved" } }),
    text_level(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"", false, G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 4, ZC_GUI_TextAlignment::Right),
    text_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"", false,
        G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 6 + G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->CalculateWstrWidth(L":") * 2, ZC_GUI_TextAlignment::Center),
    window(ZC_WOIData(text_level.GetWidth() + text_Level_saved.GetWidth() + G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) / 2.f,
        G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) * 2.f, G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) / 3.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Bottom_Pixel),
        ZC_GUI_WF__Stacionar | ZC_GUI_WF__NoBackground
        // | ZC_GUI_WF__NeedDraw)
        )
{
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Right, 0, G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) / 2.f, ZC_GUI_RowParams::Y_Center),
        { text_Level_saved.GetObj(), text_level.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) * 1.5f, ZC_GUI_RowParams::X_Right, 0.f, 0.f, ZC_GUI_RowParams::Y_Center), { text_time.GetObj() }));
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
    ecUpdater.NewConnection(ZC__Updater::Connect({ &G_GUI_W_ConfigSaveLevel::Callback_Updater, this }, G_UpdaterLevels::G_UL__w_player_win));
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