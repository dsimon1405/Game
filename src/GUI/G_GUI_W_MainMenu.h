#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include "Text/G_GUI_Text.h"
#include "Text/G_GUI_ButtonText.h"
#include "G_WindowName.h"

class G_GUI_W_MainMenu
{
public:
    G_GUI_W_MainMenu();

    void OpenWindow();
    void CloseWindow();

private:
    const float button_width = 100.f;
    const float button_height = 50.f;
    
    G_WindowName win_name = G_WN__main_menu;

    ZC_GUI__WinImmutable window;
    
    G_GUI_Text text_game;
    G_GUI_ButtonText bt_start;
    G_GUI_ButtonText bt_continue;
    G_GUI_ButtonText bt_continue_best;
    G_GUI_ButtonText bt_options;
    G_GUI_ButtonText bt_exit;

    void StartNewGame(float);
    void ContinueGame(float);
    void ContinueBestGame(float);
    void OpenOptions(float);
    void ExitGame(float);
    void ButtonFocuseChanged(bool);
};