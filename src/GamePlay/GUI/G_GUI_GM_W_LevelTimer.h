#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <System/G_Time.h>

class G_GUI_GM_W_LevelTimer : public ZC_GUI__WinImmutable
{
public:
    G_GUI_GM_W_LevelTimer();
    
    void Start();
    void SetDefaultState();
    G_Time GetTime() const noexcept;

private:
        //  don't need G_GUI_Text (have no words, only numbers)
    ZC_GUI__Text text_hours;
    ZC_GUI__Text text_minute;
    ZC_GUI__Text text_seconds;
    ZC_GUI__Text text_colon_1;
    ZC_GUI__Text text_colon_2;

    G_Time time_cur_lvl;
    float cur_time = 0.f;

    ZC_EC ecUpdater;

    void Callback_Updater(float time);
};