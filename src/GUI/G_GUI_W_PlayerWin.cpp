#include "G_GUI_W_PlayerWin.h"
    
#include <GamePlay/G_GameManager.h>
#include <ZC/GUI/ZC__GUI.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <GUI/Text/G_GUI_Fonts.h>

G_GUI_W_PlayerWin::G_GUI_W_PlayerWin()
    : window(ZC_WOIData(550.f, 360.f, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center), ZC_GUI_WF__None
    // | ZC_GUI_WF__NeedDraw),
    ),
    text_Victory(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Center, ZC_Pack_Float_To_UInt_2x10x10x10(0.f, 0.6, 0.f),
        { G_LangText{ .lang = G_L_Russian, .text = L"Победа" }, { .lang = G_L_English, .text = L"Victory" } }),
    text_Level(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Center, 0, { G_LangText{ .lang = G_L_Russian, .text = L"Уровнь" }, { .lang = G_L_English, .text = L"Level" } }),
    text_Total(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Center, 0, { G_LangText{ .lang = G_L_Russian, .text = L"Общее" }, { .lang = G_L_English, .text = L"Total" } }),
    text_Time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Left, 0, { G_LangText{ .lang = G_L_Russian, .text = L"Время:" }, { .lang = G_L_English, .text = L"Time:" } }),
    text_level_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"", false,
        G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 6 + G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->CalculateWstrWidth(L":") * 2, ZC_GUI_TextAlignment::Center),
    text_total_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"", false,
        G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->longest_number_pixels_width * 6 + G_GUI_Fonts::Get(G_GUI_FN__Arial_40)->CalculateWstrWidth(L":") * 2, ZC_GUI_TextAlignment::Center),
    bt_Go_to_main_menu(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { &G_GUI_W_PlayerWin::CallMainMenu, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"В главное меню" }, { .lang = G_L_English, .text = L"Go to main menu" } }),
    bt_Go_to_next_level(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { &G_GUI_W_PlayerWin::NextLevel, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Следующий уровень" }, { .lang = G_L_English, .text = L"Go to next level" } }),
    text_arrow(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"=>", true, 0, ZC_GUI_TextAlignment::Left)
{
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 10.f, 0.f, ZC_GUI_RowParams::Y_Center), { text_Victory.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(33.f, ZC_GUI_RowParams::X_Right, 30.f, 96.f, ZC_GUI_RowParams::Y_Center), { text_Total.GetObj(), text_Level.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 10.f, 20.f, ZC_GUI_RowParams::Y_Center),
        { text_Time.GetObj(), text_level_time.GetObj(), text_arrow.GetObj(), text_total_time.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 40.f, 0.f, ZC_GUI_RowParams::Y_Center), { bt_Go_to_next_level.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 20.f, 0.f, ZC_GUI_RowParams::Y_Center), { bt_Go_to_main_menu.GetObj() }));
}

G_GUI_W_PlayerWin::~G_GUI_W_PlayerWin()
{
    ecUpdater.Disconnect();
}

void G_GUI_W_PlayerWin::OpenWindow()
{
    window.SetDrawState(true);
        //  get times
    time_level = G_GameManager::pGM->GetLevelTime();
    time_total = G_GameManager::pGM->GetTotalTime();
    seconds_to_move = time_level.GetInSeconds();
    time_result = time_total;
    time_result.PlusSeconds(seconds_to_move);
        //  on start
    cur_time = 0;
    update_phase = UP_start;
    text_arrow.SetDrawState(false);

    text_level_time.UpdateText(CreateNumberWstr(time_level), true);
    text_total_time.UpdateText(CreateNumberWstr(time_total), true);
    ecUpdater.NewConnection(ZC__Updater::Connect({ &G_GUI_W_PlayerWin::Callback_Updater, this }, G_UpdaterLevels::G_UL__w_player_win));

    sound_win.SetSoundState(ZC_SS__Play);
}

void G_GUI_W_PlayerWin::CloseWindow()
{
    window.SetDrawState(false);
    ecUpdater.Disconnect();

    sound_win.SetSoundState(ZC_SS__Stop);
    sound_transfer_time_points.SetSoundState(ZC_SS__Stop);
}

void G_GUI_W_PlayerWin::CallMainMenu(float)
{
    G_GameManager::pGM->gui.OpenWindow(G_WN__main_menu);
}

void G_GUI_W_PlayerWin::NextLevel(float)
{
    G_GameManager::pGM->ContinueGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_PlayerWin::Callback_Updater(float time)
{
    static const float seconds_start = 0.5f;
    static const float seconds_for_update_time = 2.f;    //  second to move time from time_level to time_total
    static float cur_seconds_offset = 0.f;
    
    switch (update_phase)
    {
    case UP_start:
    {
        cur_time += time;
        if (cur_time >= seconds_start)
        {
            update_phase = UP_move;
            cur_time = 0.f;

            cur_seconds_offset = 0.f;
        }
    } break;
    case UP_move:
    {
        if (cur_time == 0.f) sound_transfer_time_points.SetSoundState(ZC_SS__Play);

        cur_time += time;
        if (cur_time >= seconds_for_update_time)
        {
            text_arrow.SetDrawState(false);
            text_level_time.UpdateText(CreateNumberWstr(G_Time{}), true);
            text_total_time.UpdateText(CreateNumberWstr(time_result), true);
            ecUpdater.Disconnect();

            sound_transfer_time_points.SetSoundState(ZC_SS__Stop);
        }
        else
        {
            text_arrow.SetDrawState(true);
            cur_seconds_offset += float(seconds_to_move) * time / seconds_for_update_time;
            if (cur_seconds_offset >= 1.f)
            {
                float offset = 0.f;
                cur_seconds_offset = std::modf(cur_seconds_offset, &offset);

                time_level.MinusSeconds(offset);
                text_level_time.UpdateText(CreateNumberWstr(time_level), true);
                time_total.PlusSeconds(offset);
                text_total_time.UpdateText(CreateNumberWstr(time_total), true);
            }
        }
    } break;
    }
}

std::wstring G_GUI_W_PlayerWin::CreateNumberWstr(G_Time time)
{
    return (time.hours > 9 ? std::to_wstring(time.hours) : L"0" + std::to_wstring(time.hours))
        + L":" + (time.minutes > 9 ? std::to_wstring(time.minutes) : L"0" + std::to_wstring(time.minutes))
        + L":" + (time.seconds > 9 ? std::to_wstring(time.seconds) : L"0" + std::to_wstring(time.seconds));
}

// std::wstring G_GUI_W_PlayerWin::GetNumber(unsigned char number)
// {
//     return number > 9 ? std::to_wstring(number) : L"0" + std::to_wstring(number);
// }