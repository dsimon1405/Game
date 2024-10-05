#pragma once

#include "G_WindowName.h"
#include "G_GUI_W_MainMenu.h"
#include "G_GUI_W_RestartLevel.h"
#include "G_GUI_W_PlayerWin.h"
#include "G_GUI_W_Options.h"

class G_GUI
{
public:
    G_GUI();
        //  Calls to close current opened window if it is, and open new window.
    void OpenWindow(G_WindowName win);
        //  Calls to close window before start game process
    void CloseCurrentWindow();

private:
    G_WindowName opened_window = G_WN__main_menu;
    
    G_GUI_W_MainMenu w_main_menu;
    G_GUI_W_Options w_options;
    G_GUI_W_RestartLevel w_restart_level;
    G_GUI_W_PlayerWin w_player_win;

    void CloseSWindowButtonClicked();
    void EscapeeClicked(ZC_ButtonID,float);

    void ChangeOpenedWindowDrawState(bool need_draw);
};