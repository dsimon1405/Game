#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include <GUI/Text/G_GUI_Text.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/Events/ZC_EC.h>
#include <System/G_Time.h>

class G_GUI_W_ConfigSaveLevel
{
public:
    G_GUI_W_ConfigSaveLevel();
    ~G_GUI_W_ConfigSaveLevel();

    // void SetDefaultState();
    void ShowSavedData(int level, G_Time time);

private:
    ZC_EC ecUpdater;

    G_GUI_Text text_Level_saved;
    ZC_GUI__Text text_level;
    ZC_GUI__Text text_time;
    ZC_GUI__WinImmutable window;

    float cur_time = 0.f;
    bool wait_to_show = true;

    void Callback_Updater(float time);
};