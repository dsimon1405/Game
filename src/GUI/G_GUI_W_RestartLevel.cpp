#include "G_GUI_W_RestartLevel.h"
    
#include <GamePlay/G_GameManager.h>
#include <GUI/Text/G_GUI_Fonts.h>

G_GUI_W_RestartLevel::G_GUI_W_RestartLevel()
    : ZC_GUI__WinImmutable(ZC_WOIData(680.f, 150.f, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center), ZC_GUI_WF__None
    // | ZC_GUI_WF__NeedDraw),
    ),
    text_fail(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), ZC_GUI_TextAlignment::Center, ZC_PackColorFloatToUInt_RGB(0.6f, 0.f, 0.f),
        { G_LangText{ .lang = G_L_Russian, .text = L"Провал" }, { .lang = G_L_English, .text = L"Fail" } }),
    bt_Go_to_main_menu(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { &G_GUI_W_RestartLevel::CallMainMenu, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"В главное меню" }, { .lang = G_L_English, .text = L"Go to main menu" } }),
    bt_restart_level(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), { &G_GUI_W_RestartLevel::RestartLevel, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Уровень заново" }, { .lang = G_L_English, .text = L"Level again" } })
{
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 10.f, 0.f, ZC_GUI_RowParams::Y_Center), { text_fail.GetObj() }));
    this->AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, 30.f, 20.f, ZC_GUI_RowParams::Y_Center), { bt_Go_to_main_menu.GetObj(), bt_restart_level.GetObj() }));
}

void G_GUI_W_RestartLevel::CallMainMenu(float)
{
    G_GameManager::pGM->gui.OpenWindow(G_WN__main_menu);
}

void G_GUI_W_RestartLevel::RestartLevel(float)
{
    G_GameManager::pGM->ContinueGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_RestartLevel::ButtonFocusChanged(bool)
{
    
}