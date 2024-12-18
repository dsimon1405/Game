#include "System/G_System.h"
#include "GamePlay/G_GameManager.h"

#include <Objects/Particles/New/G_PS_Setup.h>
#include <ZC/File/ZC_File.h>
#include <GUI/Text/G_GUI_Fonts.h>
#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <System/G_RenderLevel.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/GUI/ZC_GUI__Switch.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/GUI/ZC__GUI.h>
#include <GUI/Text/G_GUI_Fonts.h>
#include <ZC/ZC__System.h>
#include <ZC/Events/ZC_Events.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/Math/ZC_Mat.h>
#include <System/G_LightUBO.h>
#include <iostream>
#include <Model/G_ModelLoader.h>
// ZC_EC ec;
// void StartNewGame(float)
// {
//     G_GameManager::pGM->gui.w_main_menu.StartNewGame(0.f);
//     ec.Disconnect();
// }

void F(const ZC_Vec3<float>&){}

G_ParticleSystem* pPS = nullptr;

int main(int argv, char** args)
{

    G_ModelLoader::WriteToFile(G_MN__SphereMarble, { G_ModelLoader::Vertex{ .position = { 1.f, 2.f, 3.f }, .normal = 654, .texCoords = { 8.9f, 102.f } },
        G_ModelLoader::Vertex{ .position = { 11.f, 22.f, 33.f }, .normal = 44, .texCoords = { 5.5f, 6.6f } },
        G_ModelLoader::Vertex{ .position = { 111.f, 222.f, 333.f }, .normal = 444, .texCoords = { 55.5f, 66.6f } } });
    
    G_ModelLoader::ReadFile(G_MN__SphereMarble);
    
    G_System system;
    G_GameManager gm;

    // ec = ZC__Updater::Connect({StartNewGame}, 0);

    // G_Camera cam_test(ZC_Function<void(const ZC_Vec3<float>&)>{&F}, ZC_Vec3<float>{ 0.f, 0.f, 0.f });
    // cam_test.SetConnectionToEvents(true);

    // G_CubeMap cm;

    system.RunMainCycle();
    
    return 0;
}

//  G_Star::Callback_Updater(), G_PlatformDamage::Callback_Updater(), G_Section::GetRandomPlatform(), G_Section::Callback_Updater()

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  mouse capture,   full screen system


//  need to edit ZC_GUI_TextInputWindow::Caret movement with pressed ctr and collision on spesial symbols (.,*!$ ...), in method ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown/KeyboardButtonRightDown(bool needSubData)
        // //      aaaaaa&&&.&#$   .**#
        //  ckeck what is the symbol with > < = of unicode codes! with not a == L'#' !!!
        // enum Chars
        // {
        //     Ch_symbol,
        //     Ch_text,
        //     Ch_space
        // } cur_char;     //  void ZC_GUI_TextInputWindow::Caret::KeyboardButtonLeftDown(bool needSubData)