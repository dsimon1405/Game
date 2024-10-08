#pragma once

#include <ZC/Objects/Text/ZC_TextWindow.h>
#include <Sound/G_GameSound.h>

class G_GUI_GM_StartTimer
{
public:
    G_GUI_GM_StartTimer();

    void SetDefaultState();
        //  make update numbers 3,2,1 on start. If update in process return false, if finished otherwise true.
    bool Update(float time);
        //  return true if timer finished counting 3,2,1
    bool IsFinished();

private:
    float cur_time = 0.f;
    
    ZC_TextWindow tw_start_counter[3];
    unsigned char tw_index = 0;
    
    G_GameSound sound_timer;
};