#pragma once

#include "G_WindowName.h"
#include "G_GUI_W_MainMenu.h"
#include "G_GUI_W_RestartLevel.h"
#include "G_GUI_W_PlayerWin.h"
#include "G_GUI_W_Options.h"
#include <Sound/G_SystemSound.h>

class G_GUI
{
public:
    G_GUI();
        //  Calls to close current opened window if it is, and open new window.
    void OpenWindow(G_WindowName win);
        //  Calls to close window before start game process
    void CloseCurrentWindow();

// private:
    G_WindowName opened_window = G_WN__main_menu;

    bool try_close_game = false;
    
    G_GUI_W_MainMenu w_main_menu;
    G_GUI_W_Options w_options;
    G_GUI_W_RestartLevel w_restart_level;
    G_GUI_W_PlayerWin w_player_win;

    G_SystemSound ss_open_window { G_SN__gui_button_clicked, 100.f };

    void CloseSWindowButtonClicked();
    void EscapeeClicked(ZC_ButtonID,float);
    void GoBackToMainMenu();

    void ChangeOpenedWindowDrawState(bool need_draw);
};