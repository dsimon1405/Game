#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include "Text/G_GUI_Text.h"
#include "Text/G_GUI_ButtonText.h"
#include <System/G_Time.h>
#include "G_WindowName.h"

class G_GUI_W_PlayerWin
{
public:
    G_GUI_W_PlayerWin();
    ~G_GUI_W_PlayerWin();

    void OpenWindow();
    void CloseWindow();

private:
    const float button_width = 400.f;
    const float button_height = 50.f;
    
    G_WindowName win_name = G_WN__player_win;

    ZC_EC ecUpdater;

    ZC_GUI__WinImmutable window;
    G_GUI_Text text_win;
    G_GUI_Text text_time_level;
    G_GUI_Text text_time_total;
    G_GUI_Text text_time;
    ZC_GUI__Text text_time_level_number;
    ZC_GUI__Text text_time_total_number;
    G_GUI_ButtonText bt_main_menu;
    G_GUI_ButtonText bt_next_level;
    ZC_GUI__Text text_arrow;

    enum UpdatePhase
    {
        UP_start,    //  wait to start update
        UP_move    //  move time from time_level to time_total
    } update_phase = UP_start;

    G_Time time_level;
    G_Time time_total;
    G_Time time_result;     //  must be in time_total in the end of updater phase
    int seconds_to_move = 0; //  seconds to move from time_level to time_total
    float cur_time = 0.f;

    void CallMainMenu(float);
    void NextLevel(float);
    void ButtonFocuseChanged(bool);
    
    void Callback_Updater(float time);

    std::wstring CreateNumberWstr(G_Time time);
    // std::wstring GetNumber(unsigned char number);
};