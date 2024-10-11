#include "System/G_Window.h"
#include "GamePlay/G_GameManager.h"

#include <ZC/Audio/ZC_Sounds.h>
// #include "System/G_SoundName.h"
#include <ZC/Events/ZC_Events.h>
#include <ZC/Tools/ZC_Random.h>

#include <Objects/Particles/G_Particles.h>
#include <Objects/Particles/G_PS_Wind.h>
#include <System/G_RenderLevel.h>

G_PS_Wind* pP;
#include <iostream>
float speed = 10.f;
void A_plus(ZC_ButtonID,float)
{
    pP->SetSpeed(speed += 0.1f);
    std::cout<<speed<<std::endl;
}
void A_minus(ZC_ButtonID,float)
{
    pP->SetSpeed(speed -= 0.1f);
    std::cout<<speed<<std::endl;
}
float alpha = 1.f;
void S_plus(ZC_ButtonID,float)
{
    if (alpha + 0.1f <= 1.f) pP->SetAlpha(alpha += 0.1f);
    std::cout<<alpha<<std::endl;
}
void S_minus(ZC_ButtonID,float)
{
    if (alpha - 0.1f >= 0.f) pP->SetAlpha(alpha -= 0.1f);
    std::cout<<alpha<<std::endl;
}
float angle_external = 0.f;
// ZC_Vec3<float> pos{ 0.f, 0.f, 1.f };
ZC_Vec3<float> pos{ 10.f, 0.f, 0.01f };
ZC_Vec4<float> start_pos{ pos, 1.f };
void An_plus(ZC_ButtonID,float)
{
    pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle_external += 4.f, {0.f, 0.f, 1.f}) * start_pos);
    pP->SetStartPosition(pos);
}
void An_minus(ZC_ButtonID,float)
{
    pos = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle_external -= 4.f, {0.f, 0.f, 1.f}) * start_pos);
    pP->SetStartPosition(pos);
}
ZC_Vec3<float> dir_start{ -1.f, 0.f, 0.f };
ZC_Vec4<float> dir_start1{dir_start, 1.f};
void At_plus(ZC_ButtonID,float)
{
    pP->SetWindDiraction(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle_external += 4.f, {0.f, 0.f, 1.f}) * dir_start1));
}
void At_minus(ZC_ButtonID,float)
{
    pP->SetWindDiraction(ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(angle_external -= 4.f, {0.f, 0.f, 1.f}) * dir_start1));
}


int main(int argv, char** args)
{
    G_Window window;
    G_GameManager gm;

    // G_PS_Wind ps_wind(5000, pos, 15.f, 4.f, dir_start, 30.f);
    // ps_wind.SetSpeed(10.f);
    // ps_wind.SetDrawState(true);
    // pP = &ps_wind;

    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_Y, { A_plus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_U, { A_minus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_N, { S_plus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_M, { S_minus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_O, { An_plus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_P, { An_minus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_K, { At_plus }, nullptr);
    // ZC_Events::ConnectButtonClick(ZC_ButtonID::K_L, { At_minus }, nullptr);


    window.RunMainCycle();

    return 0;
}

//  timer,  dmg platform,    cursor capture,    camera scroll dist,     platform rotation,  platforms same,     switch win platform,    wind push,  wind power max
//  SCALING LARGE SPHERE (MAP), on large level