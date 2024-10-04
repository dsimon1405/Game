#include "G_PlatformWin.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <GamePlay/G_GameManager.h>
#include <System/G_Func.h>

G_PlatformWin::G_PlatformWin(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0)
{
    ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformWin::Callback_Updater, this }, G_UL__game_play));
}

void G_PlatformWin::VAddObjectOnPlatform(G_Object* pObj_add)
{
    if (pObj_add->VGetType_O() & G_OT__Player)
    {
        ch_d.state = S_win;
        ch_d.time = 0.f;
        objects_on_platform.emplace_back(pObj_add);
    }
}

void G_PlatformWin::Callback_Updater(float time)
{
    ch_d.time += time;
    switch (ch_d.state)
    {
    case S_activate:
    {
        if (ch_d.time >= seconds_activate)
        {
            ch_d.time = 0.f;
            ch_d.state = S_active;
            this->unColor = win_color_packed;
        }
        else this->unColor = G_InterpolateColor(G_Platform::start_lightning_color, win_color, ch_d.time / seconds_activate);
    } break;
    case S_active:
    {
        if (ch_d.time >= seconds_active)
        {
            ch_d.time = 0.f;
            ch_d.state = S_deactivate;
        }
    } break;
    case S_deactivate:
    {
        if (ch_d.time >= seconds_activate)
        {
            ch_d.time = 0.f;
            ch_d.state = S_activate;
            this->unColor = win_color_packed;
            //  ask map to change pos
        }
        else this->unColor = G_InterpolateColor(win_color, G_Platform::start_lightning_color, ch_d.time / seconds_activate);
    } break;
    case S_win:
    {
        static const float seconds_win_blick = 0.5f;
        static const unsigned char win_blick_times = 4;
        if (ch_d.time >= seconds_win_blick)
        {
            ch_d.time = 0.f;
            ++ch_d.win_blicks_count;
            if (ch_d.win_blicks_count == win_blick_times)
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
                else ch_d = { .state = S_deactivate };
            }
        }
        else this->unColor = int(ch_d.time) % 2 != 0 ? G_InterpolateColor(win_color, G_Platform::start_lightning_color, ch_d.time / seconds_win_blick)
            : G_InterpolateColor(G_Platform::start_lightning_color, win_color, ch_d.time / seconds_win_blick);
    } break;
    }
}