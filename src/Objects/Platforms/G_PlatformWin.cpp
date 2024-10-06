#include "G_PlatformWin.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <GamePlay/G_GameManager.h>
#include <System/G_Func.h>

G_PlatformWin::G_PlatformWin(const G_PlatformTransforms& _plat_trans, ZC_Function<void(G_Platform*)>&& _func_change_pos)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0),
    func_change_pos(std::move(_func_change_pos))
{
    ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformWin::Callback_Updater, this }, G_UL__game_play));
}

void G_PlatformWin::Update_func_change_pos(ZC_Function<void(G_Platform*)>&& _func_change_pos)
{
    func_change_pos = std::move(_func_change_pos);
}

void G_PlatformWin::VAddObjectOnPlatform(G_Object* pObj_add)
{
    if (pObj_add->VGetType_O() & G_OT__Player)
    {
        ch_d = { ch_d.state = S_win, ch_d.time = 0.f, ch_d.color = ZC_UnpackUINTtoFloat_RGB(this->unColor) };
        objects_on_platform.emplace_back(pObj_add);
    }
}

void G_PlatformWin::Callback_Updater(float time)
{
    static const float seconds_activate = 3.f;
    static const float seconds_active = 20.f;
    static const ZC_Vec3<float> color_win { 0.f, 1.f, 0.f };
    static const uint win_color_packed = ZC_PackColorFloatToUInt_RGB(color_win[0], color_win[1], color_win[2]);

    ch_d.time += time;
    switch (ch_d.state)
    {
    case S_activate:
    {
        if (ch_d.time >= seconds_activate)
        {
            ch_d = { .state = S_active, .time = 0.f, .color = G_Platform::color_white };
            this->unColor = packed_active_color;
        }
        else this->unColor = G_InterpolateColor(ch_d.color, G_Platform::color_white, ch_d.time / seconds_activate);
    } break;
    case S_active: if (ch_d.time >= seconds_active) ch_d = { .state = S_deactivate, .time = 0.f, .color = G_Platform::color_white }; break;
    case S_deactivate:
    {
        if (ch_d.time >= seconds_activate)
        {
            ch_d = { .state = S_activate, .time = 0.f, .color = G_Platform::color_default };
            this->unColor = G_Platform::color_default_packed;
            func_change_pos(this);  //  switch to new position
        }
        else this->unColor = G_InterpolateColor(ch_d.color, G_Platform::color_default, ch_d.time / seconds_activate);
    } break;
    case S_win:
    {
        static const float seconds_win_activate = 2.f;

        if (ch_d.time >= seconds_win_activate)
        {
            this->unColor = win_color_packed;
            G_Object* pObj_player = nullptr;
            for (G_Object* pObj : objects_on_platform)
            {
                if (pObj->VGetType_O() & G_OT__Player)
                {
                    pObj_player = pObj;
                    break;
                }
            }
            if (pObj_player && this->IsObjectInPlatformRadiusXY(pObj_player)) G_GameManager::pGM->PlayerWin();
            else ch_d = { .state = S_deactivate, .time = 0.f, .color = color_win };
        }
        else this->unColor = G_InterpolateColor(ch_d.color, color_win, ch_d.time / seconds_win_activate);
    } break;
    }
}