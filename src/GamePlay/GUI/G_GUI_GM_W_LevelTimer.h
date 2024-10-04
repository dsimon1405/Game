#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/ZC_GUI__Text.h>

class G_GUI_GM_W_LevelTimer : public ZC_GUI__WinImmutable
{
public:
    G_GUI_GM_W_LevelTimer();

    void UpdateHours(const std::wstring& text);
    void UpdateMinutes(const std::wstring& text);
    void UpdateSeconds(const std::wstring& text);

private:
        //  don't need G_GUI_Text (have no words, only numbers)
    ZC_GUI__Text text_hours;
    ZC_GUI__Text text_minute;
    ZC_GUI__Text text_seconds;
    ZC_GUI__Text text_colon_1;
    ZC_GUI__Text text_colon_2;
};