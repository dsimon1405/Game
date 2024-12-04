#include "G_GUI_W_Options.h"

#include <GamePlay/G_GameManager.h>
#include <ZC/ZC__System.h>
#include <System/G_Config.h>
#include <ZC/GUI/ZC__GUI.h>
#include <GUI/Text/G_GUI_Fonts.h>

G_GUI_W_Options::G_GUI_W_Options()
    : ZC_GUI__WinImmutable(ZC_WOIData(500.f, 430.f, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center), ZC_GUI_WF__None
    //  | ZC_GUI_WF__NeedDraw),
    ),
    text_Options(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Center, 0, { G_LangText{ .lang = G_L_Russian, .text = L"НАСТРОЙКИ" }, { .lang = G_L_English, .text = L"OPTIONS" } }),
    text_Language(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Left, 0, { G_LangText{ .lang = G_L_Russian, .text = L"Язык:" }, { .lang = G_L_English, .text = L"Language:" } }),
    sdd_language(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { L"English", L"Русский" }, G_Config::GetConfigData().language, 0.f, 0.f, { &G_Config::Update_language },
        ZC_GUI_ColorsDropDown(ZC_GUI_ColorsButton(ZC_PackColorUCharToUInt_RGB(40, 40, 40), ZC_PackColorUCharToUInt_RGB(60, 60, 60),
            ZC_PackColorUCharToUInt_RGB(80, 80, 80), ZC_PackColorUCharToUInt_RGB(100, 100, 100)), ZC_PackColorUCharToUInt_RGB(200, 200, 200),
            ZC_PackColorUCharToUInt_RGB(150, 150, 150))),
    chB_fullscreen(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), G_Config::GetConfigData().fuull_screen, { &G_Config::Update_full_screen }, false, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Весь экран" }, { .lang = G_L_English, .text = L"Full screen" } }),
    bit_volume(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), int(G_Config::GetConfigData().volume_coef * 100.f), 0, 100, { &G_Config::Update_volume }, false, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Звук" }, { .lang = G_L_English, .text = L"Sound" } }),
    bt_Go_to_main_menu(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { &G_GUI_W_Options::CallMainMenu, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"В главное меню" }, { .lang = G_L_English, .text = L"Go to main menu" } })
{
    const float distance_y = 20.f;
    const float offset_x = 40.f;
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, distance_y, 0.f, ZC_GUI_RowParams::Y_Center), { text_Options.GetObj() }));
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(offset_x, ZC_GUI_RowParams::X_Left, 50.f, G_GUI_Fonts::GetFontHeight(G_GUI_FN__Arial_40) / 2.f, ZC_GUI_RowParams::Y_Center),
        { text_Language.GetObj(), &sdd_language }));
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(offset_x, ZC_GUI_RowParams::X_Left, distance_y, 0.f, ZC_GUI_RowParams::Y_Center), { chB_fullscreen.GetObj() }));
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(offset_x, ZC_GUI_RowParams::X_Left, distance_y, 0.f, ZC_GUI_RowParams::Y_Center), { bit_volume.GetObj() }));
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 70.f, 0.f, ZC_GUI_RowParams::Y_Center), { bt_Go_to_main_menu.GetObj() }));
}

void G_GUI_W_Options::CallMainMenu(float)
{
    G_GameManager::pGM->gui.OpenWindow(G_WN__main_menu);
}