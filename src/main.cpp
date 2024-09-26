#include <ZC/Video/ZC_SWindow.h>

#include <G_GameManager.h>
#include <ZC/Events/ZC_ESignal.h>

int main(int argv, char** args)
{
    using namespace ZC_SWindow;
    ZC_SWindow::MakeWindow(
        ZC_SW__Multisampling_4 | 
        ZC_SW__Border
        //  | ZC_SW__GUI
         , 800, 600, "Game");
    // ZC_SWindow::SetFPS(0);
    ZC_SWindow::NeedDrawFPS(true);
    ZC_SWindow::SetFPSTimeMeasure(ZC_FPS_TM__Seconds);
    ZC_SWindow::GlClearColor(0.5, 0.5, 0.5, 1.f);

    G_GameaManager gm;
    
    ZC_SWindow::RunMainCycle();

    return 0;
}