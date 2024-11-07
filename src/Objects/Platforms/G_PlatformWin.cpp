#include "G_PlatformWin.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <GamePlay/G_GameManager.h>
#include <System/G_Func.h>

G_PlatformWin::G_PlatformWin(const G_PlatformTransforms& _plat_trans, ZC_Function<void(G_Platform*)>&& _func_change_pos)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()), G_PT__Wind),
    func_change_pos(std::move(_func_change_pos))
{
    ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformWin::Callback_Updater, this }, G_UL__game_play));
}

void G_PlatformWin::Update_func_change_pos(ZC_Function<void(G_Platform*)>&& _func_change_pos)
{
    func_change_pos = std::move(_func_change_pos);
}

void G_PlatformWin::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    if (ch_d.state == S_player_move_to_next_level) return;
    if (pObj_add->VGetTypeMask_IO() & G_OT__Player)
    {
        ch_d = { ch_d.state = S_win, ch_d.time = 0.f, ch_d.color = ZC_UnpackUINTtoFloat_RGB(this->unColor) };
        objects_on_platform.emplace_back(pObj_add);
        this->upSK->SetSoundState(G_SN__platform_win, ZC_SS__Play);
    }
}

std::vector<G_GameSound> G_PlatformWin::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(1);
    // sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_win));
    return sounds;
}

void G_PlatformWin::Callback_Updater(float time)
{
    static const float seconds_activate = 3.f;
    static const float seconds_active = 10.f;
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
        else this->unColor = G_InterpolateColor_PackToUInt(ch_d.color, G_Platform::color_white, ch_d.time / seconds_activate);
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
        else this->unColor = G_InterpolateColor_PackToUInt(ch_d.color, G_Platform::color_default, ch_d.time / seconds_activate);
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
                if (pObj->VGetTypeMask_IO() & G_OT__Player)
                {
                    pObj_player = pObj;
                    break;
                }
            }
            if (pObj_player && this->IsObjectInPlatformRadiusXY(pObj_player))
            {
                G_GameManager::pGM->PlayerWin();
                ch_d.state = S_player_move_to_next_level;
                ecUpdater.Disconnect();
            }
            else
            {
                ch_d = { .state = S_deactivate, .time = 0.f, .color = color_win };
                this->upSK->SetSoundState(G_SN__platform_win, ZC_SS__Play);
            }
        }
        else this->unColor = G_InterpolateColor_PackToUInt(ch_d.color, color_win, ch_d.time / seconds_win_activate);
    } break;
    case S_player_move_to_next_level: break;
    }
}