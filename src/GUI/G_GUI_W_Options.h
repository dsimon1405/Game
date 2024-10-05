#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include "Text/G_GUI_Text.h"
#include "Buttons/G_GUI_CheckBox.h"
#include "Buttons/G_GUI_ButtonInt32Text.h"
#include "Buttons/G_GUI_ButtonText.h"
#include <ZC/GUI/ZC_GUI__Switch.h>
#include "G_WindowName.h"

class G_GUI_W_Options : public ZC_GUI__WinImmutable
{
public:
    G_GUI_W_Options();

private:
    G_WindowName win_name = G_WN__options;
    
    G_GUI_Text text_Options;
    G_GUI_Text text_Language;
    ZC_GUI_SwitchDropDown sdd_language;
    G_GUI_CheckBox chB_fullscreen;
    G_GUI_ButtonInt32Text bit_volume;
    G_GUI_ButtonText bt_Go_to_main_menu;

    void CallMainMenu(float);
};