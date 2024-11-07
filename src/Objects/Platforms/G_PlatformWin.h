#pragma once

#include <ZC/Events/ZC_EC.h>
#include "G_Platform.h"

class G_PlatformWin : public G_Platform
{
public:
    G_PlatformWin(const G_PlatformTransforms& _plat_trans, ZC_Function<void(G_Platform*)>&& _func_change_pos);

    void Update_func_change_pos(ZC_Function<void(G_Platform*)>&& _func_change_pos);

private:
    enum State
    {
        S_activate,
        S_active,
        S_deactivate,
        S_win,
        S_player_move_to_next_level
    };
    
    ZC_Function<void(G_Platform*)> func_change_pos;

    struct ChangableData
    {
        State state = S_activate;
        float time = 0.f;
        // unsigned char win_blicks_count = 0;
        ZC_Vec3<float> color = G_Platform::color_default;
    } ch_d;

    void VAddObjectOnPlatform_P(G_Object* pObj_add) override;
    
    std::vector<G_GameSound> GetSounds();
    void Callback_Updater(float time);
};