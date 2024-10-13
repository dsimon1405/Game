#pragma once

#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <ZC/Events/ZC_EC.h>
#include <ZC/Objects/Text/ZC_TextWindow.h>
#include <Objects/G_ObjectType.h>

class G_GUI_P_W_Health
{
public:
    G_GUI_P_W_Health(int _hp);
    ~G_GUI_P_W_Health();

    void SetDefaultState();
    void UpdateHealth(int hp_new, G_ObjectType ot_damager);

private:
    const ZC_Vec3<float> color_hp_max { 0.f, 0.8f, 0.f };
    const unsigned int color_hp_max_pack = ZC_PackColorFloatToUInt_RGB(color_hp_max);
    const ZC_Vec3<float> color_hp_min { 0.8f, 0.f, 0.f };
    const unsigned int color_hp_min_pack = ZC_PackColorFloatToUInt_RGB(color_hp_min);

    ZC_GUI__WinImmutable window;

    std::list<int> new_hps; //  hp must stand after dmg

    ZC_EC ecUpdater;

        //  don't need G_GUI_Text (have no words, only numbers)
    ZC_GUI__Text text_health;
    std::list<ZC_uptr<ZC_TextWindow>> hp_substracts;

    const int hp_default;
    int cur_hp = hp_default;
    float minus_hp_fractional = 0.f;

    float subtract_indent_X = 0.f;
    float subtract_indent_Y = 0.f;
    bool subtract_indents_need_update = true;

    void Callback_Updater(float time);
    void CalculateSubtractIndents();
};