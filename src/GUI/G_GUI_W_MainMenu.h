#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include "Text/G_GUI_Text.h"
#include "Buttons/G_GUI_ButtonText.h"
#include "G_WindowName.h"

class G_GUI_W_MainMenu
{
public:
    G_GUI_W_MainMenu();

    void OpenWindow();
    void CloseWindow();

// private:
    G_WindowName win_name = G_WN__main_menu;

    ZC_GUI__WinImmutable window;
    
    G_GUI_Text text_game;
    G_GUI_ButtonText bt_start;
    G_GUI_ButtonText bt_continue;
    G_GUI_ButtonText bt_continue_best;
    G_GUI_ButtonText bt_options;
    G_GUI_ButtonText bt_exit;

    bool game_was_launched = false;

    void StartNewGame(float);
    void ContinueGame(float);
    void ContinueBestGame(float);
    void OpenOptions(float);
    void ExitGame(float);
    void ButtonFocusChanged(bool);
};