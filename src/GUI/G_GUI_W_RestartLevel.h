#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include "Text/G_GUI_Text.h"
#include "Buttons/G_GUI_ButtonText.h"
#include "G_WindowName.h"

class G_GUI_W_RestartLevel : public ZC_GUI__WinImmutable
{
public:
    G_GUI_W_RestartLevel();

private:
    G_WindowName win_name = G_WN__restart_level;
    
    G_GUI_Text text_fail;
    G_GUI_ButtonText bt_Go_to_main_menu;
    G_GUI_ButtonText bt_restart_level;

    void CallMainMenu(float);
    void RestartLevel(float);
    void ButtonFocusChanged(bool);
};