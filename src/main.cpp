#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

int main(int argv, char** args)
{
    G_System system;
    G_GameManager gm;

    system.RunMainCycle();
    
    return 0;
}

//  G_Star::Callback_Updater(), G_PlatformDamage::Callback_Updater(), G_Section::GetRandomPlatform(), G_Section::Callback_Updater()

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  full screen system


//  need to edit ZC_GUI_TextInputWindow::Caret movement with pressed ctr and collision on spesial symbols (.,*!$ ...), in method ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown/KeyboardButtonRightDown(bool needSubData)
        // //      aaaaaa&&&.&#$   .**#
        //  ckeck what is the symbol with > < = of unicode codes! with not a == L'#' !!!
        // enum Chars
        // {
        //     Ch_symbol,
        //     Ch_text,
        //     Ch_space
        // } cur_char;     //  void ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown(bool needSubData)