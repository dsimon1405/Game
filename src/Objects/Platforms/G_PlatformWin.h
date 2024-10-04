#pragma once

#include <ZC/Events/ZC_EC.h>
#include "G_Platform.h"

class G_PlatformWin : public G_Platform
{
public:
    G_PlatformWin(const G_PlatformTransforms& _plat_trans);

private:
    static inline const float seconds_activate = 4.f;
    static inline const float seconds_active = 10.f;
    static inline const ZC_Vec3<float> win_color { 0.f, 1.f, 0.f };
    static inline const uint win_color_packed = ZC_PackColorFloatToUInt_RGB(win_color[0], win_color[1], win_color[2]);

    enum State
    {
        S_activate,
        S_active,
        S_deactivate,
        S_win,
    };

    struct ChangableData
    {
        State state = S_activate;
        float time = 0.f;
        unsigned char win_blicks_count = 0;
    } ch_d;

    void VAddObjectOnPlatform(G_Object* pObj_add) override;
    
    void Callback_Updater(float time);
};