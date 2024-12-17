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
ZC_EC ec;
void StartNewGame(float)
{
    G_GameManager::pGM->gui.w_main_menu.StartNewGame(0.f);
    ec.Disconnect();
}

float offset_linear = 0.f;
float offset_quadratic = 0.f;

void CalculateOffsets()
{
    offset_linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear / 5.f;
    offset_quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic / 5.f;
}

void I_minus_linear(ZC_ButtonID,float)
{
    if (offset_linear == 0.f) CalculateOffsets();
    std::cout << "linear: " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << " - " << offset_linear;
    G_LightUBO::UpdateAttenuation(G_LAN_CubeMap, G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear - offset_linear, .quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic });
    std::cout << " = " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << std::endl;
}

void I_plus_linear(ZC_ButtonID,float)
{
    if (offset_linear == 0.f) CalculateOffsets();
    std::cout << "linear: " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << " + " << offset_linear;
    G_LightUBO::UpdateAttenuation(G_LAN_CubeMap, G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear + offset_linear, .quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic });
    std::cout << " = " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << std::endl;
}

void I_minus_quadratic(ZC_ButtonID,float)
{
    if (offset_quadratic == 0.f) CalculateOffsets();
    std::cout << "quadratic: " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << " - " << offset_quadratic;
    G_LightUBO::UpdateAttenuation(G_LAN_CubeMap, G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear, .quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic - offset_quadratic });
    std::cout << " = " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic << std::endl;
}

void I_plus_quadratic(ZC_ButtonID,float)
{
    if (offset_quadratic == 0.f) CalculateOffsets();
    std::cout << "quadratic: " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear << " + " << offset_quadratic;
    G_LightUBO::UpdateAttenuation(G_LAN_CubeMap, G_LightAttenuation{ .linear = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].linear, .quadratic = G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic + offset_quadratic });
    std::cout << " = " << G_LightUBO::c_light.attenuations[G_LAN_CubeMap].quadratic << std::endl;
}

void F(const ZC_Vec3<float>&){}

G_ParticleSystem* pPS = nullptr;

int main(int argv, char** args)
{
    G_System system;
    G_GameManager gm;

    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_I, { &I_minus_linear }, nullptr);
    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_O, { &I_plus_linear }, nullptr);
    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_K, { &I_minus_quadratic }, nullptr);
    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_L, { &I_plus_quadratic }, nullptr);
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