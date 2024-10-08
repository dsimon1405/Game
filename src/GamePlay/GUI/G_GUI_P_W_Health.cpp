#include "G_GUI_P_W_Health.h"

#include <ZC/GUI/ZC__GUI.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_Func.h>
#include <System/G_FontData.h>

#include <cmath>

G_GUI_P_W_Health::G_GUI_P_W_Health(int _hp)
    : window(ZC_WOIData(ZC__GUI::GetLongestNumberCharacterWidth() * 3, ZC__GUI::GetFontHeight(), 0.f, ZC__GUI::GetFontHeight(), ZC_WOIF__X_Center | ZC_WOIF__Y_Bottom_Pixel),
        ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Stacionar | ZC_GUI_WF__NoBackground),
    text_health(std::to_wstring(_hp), false, ZC__GUI::GetLongestNumberCharacterWidth() * 3, ZC_GUI_TextAlignment::Center, color_hp_max_pack),
    hp_default(_hp)
{
    window.AddRow(ZC_GUI_Row(ZC_GUI_Row(ZC_GUI_RowParams(0, ZC_GUI_RowParams::Center, 0, 0), { text_health.GetObj() })));
}

G_GUI_P_W_Health::~G_GUI_P_W_Health()
{
    ecUpdater.Disconnect();
}

void G_GUI_P_W_Health::SetDefaultState()
{
    new_hps.clear();
    ecUpdater.Disconnect();
    
    text_health.SetColor(color_hp_max_pack);
    text_health.UpdateText(std::to_wstring(hp_default), true);

    hp_substracts.clear();

    cur_hp = hp_default;
}

void G_GUI_P_W_Health::UpdateHealth(int hp_new)
{
    int subtract_hp = 0;
    if (new_hps.empty())
    {
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_GUI_P_W_Health::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play));
        subtract_hp = hp_new - cur_hp;
        minus_hp_fractional = 0.f;
    }
    else subtract_hp = hp_new - new_hps.back();
        //  add subtract number
    ZC_uptr<ZC_TextWindow>& tw_subtract = hp_substracts.emplace_back(
            new ZC_TextWindow(G_FontData::fd_Arial_30, std::to_wstring(subtract_hp), ZC_TA_Center, 0.f, text_health.GetPositionBL()[1] + text_health.GetHeight(),
                ZC_WOIF__X_Center | ZC_WOIF__Y_Bottom_Pixel, true));
    // ZC_uptr<ZC_TextWindow>& tw_subtract = hp_substracts.emplace_back(
    //         new ZC_TextWindow(G_FontData::fd_Arial_30, std::to_string(subtract_hp), ZC_TA_Center, 0, 0, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Bottom_Pixel, true));
    //     //  calculate subtract text indents from right and buttom borders
    // ZC_Vec2<float> health_pos = text_health.GetPositionBL();
    // float offset_x = text_health.GetWidth() - tw_subtract->GetWidth();
    // float tw_X = health_pos[0] + (offset_x >= 0.f ? (offset_x) / 2.f : offset_x) + tw_subtract->GetWidth();  //  pos in window from bl corner
    // float tw_Y = health_pos[1] + text_health.GetHeight();  //  pos in dindow from bl corner
    // int swindow_width = 0.f;
    // int swindow_height = 0.f;
    // ZC_SWindow::GetSize(swindow_width, swindow_height);
    // float tw_indent_right = swindow_width - tw_X;
    // tw_subtract->SetNewIndentParams(tw_indent_right, tw_Y, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Bottom_Pixel);
    tw_subtract->SetColorFloat(color_hp_min[0], color_hp_min[1], color_hp_min[2]);

    new_hps.emplace_back(hp_new);
}

void G_GUI_P_W_Health::Callback_Updater(float time)
{
    static const float hp_remove_speed = 30.f / 1.f;
        //  move hp from preious to new
    if (!new_hps.empty())
    {
        minus_hp_fractional += hp_remove_speed * time;
        if (minus_hp_fractional >= 1.f)     //  can be divided into integer and fractional parts
        {
            float minus_hp_integral = 0.f;    //  store integral part of minus_hp
            minus_hp_fractional = std::modf(minus_hp_fractional, &minus_hp_integral);   //  store fractionral part of  minus_hp
                //  find correct hp
            int new_hp = cur_hp - minus_hp_integral;
            int must_be_hp = new_hp;    //  will store correct h[]
            for (auto iter = new_hps.begin(); iter != new_hps.end();)
            {
                if (new_hp <= *iter)
                {
                    must_be_hp = *iter;
                    iter = new_hps.erase(iter);
                }
                else
                {
                    must_be_hp = new_hp;
                    break;
                }
            }
            cur_hp = must_be_hp;
            text_health.UpdateText(std::to_wstring(cur_hp), true);

                //  cur_hp color color
            static const ZC_Vec3<float> color_midle(color_hp_min[0], color_hp_max[1], 0.f);
            static const uint color_midle_packed = ZC_PackColorFloatToUInt_RGB(color_midle);
            
            static const float half_hp = hp_default / 2;
            if (cur_hp > half_hp) text_health.SetColor(G_InterpolateColor(color_hp_max, color_midle, 1.f - ((float(cur_hp) - half_hp) / half_hp)));
            else if (cur_hp < half_hp) text_health.SetColor(G_InterpolateColor(color_midle, color_hp_min, 1.f - (float(cur_hp) / half_hp)));
            else text_health.SetColor(color_midle_packed);
        }
    }

    if (!(hp_substracts.empty()))
    {
        static const float alpha_speed = 1.f / 2.f;     //  alpha disapearing in two seconds
        static const float y_speed = 50.f / 1.f;     //  move on 10 pixel in second
        
        for (auto iter = hp_substracts.begin(); iter != hp_substracts.end(); )
        {       //  minus alpha
            float alpha = (*iter)->GetAlpha();
            float new_alpha = alpha - (alpha_speed * time);
            if (new_alpha <= 0.f) iter = hp_substracts.erase(iter);
            else
            {
                (*iter)->SetAlpha(new_alpha);
                
                    //  plus Y (go up)
                float ind_x = 0.f;
                float ind_y = 0.f;
                int flags = 0;
                (*iter)->GetIndentParams(ind_x, ind_y, flags);
                (*iter)->SetNewIndentParams(ind_x, ind_y + (y_speed * time), flags);
                
                ++iter;
            }
        }
    }
        //  stop update
    if (new_hps.empty() && hp_substracts.empty()) ecUpdater.Disconnect();
}