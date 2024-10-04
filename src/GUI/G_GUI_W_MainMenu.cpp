#include "G_GUI_W_MainMenu.h"

#include <GamePlay/G_GameManager.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>

G_GUI_W_MainMenu::G_GUI_W_MainMenu()
    : window(ZC_WOIData(500.f, 370.f, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center), ZC_GUI_WF__Stacionar
    //  | ZC_GUI_WF__NeedDraw),
    ),
    text_game(ZC_GUI_TextAlignment::Center, 0, { G_LangText{ .lang = G_L_Russian, .text = L"И-Г-Р-А" }, { .lang = G_L_English, .text = L"G-A-M-E" } }),
    bt_start(button_width, button_height, { &G_GUI_W_MainMenu::StartNewGame, this }, { &G_GUI_W_MainMenu::ButtonFocuseChanged, this }, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Новая игра" }, { .lang = G_L_English, .text = L"New game" } }),
    bt_continue(button_width, button_height, { &G_GUI_W_MainMenu::ContinueGame, this }, { &G_GUI_W_MainMenu::ButtonFocuseChanged, this }, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Продолжить игру" }, { .lang = G_L_English, .text = L"Continue game" } }),
    bt_continue_best(button_width, button_height, { &G_GUI_W_MainMenu::ContinueBestGame, this }, { &G_GUI_W_MainMenu::ButtonFocuseChanged, this }, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Продолжить сохранённую" }, { .lang = G_L_English, .text = L"Continue saved" } }),
    bt_options(button_width, button_height, { &G_GUI_W_MainMenu::OpenOptions, this }, { &G_GUI_W_MainMenu::ButtonFocuseChanged, this }, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Настройки" }, { .lang = G_L_English, .text = L"Options" } }),
    bt_exit(button_width, button_height, { &G_GUI_W_MainMenu::ExitGame, this }, { &G_GUI_W_MainMenu::ButtonFocuseChanged, this }, {},
        { G_LangText{ .lang = G_L_Russian, .text = L"Покинуть игру" }, { .lang = G_L_English, .text = L"Leave the game" } })
{
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { text_game.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { bt_start.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { bt_continue.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { bt_continue_best.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { bt_options.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, 10.f, 0.f), { bt_exit.GetObj() }));

    bt_continue.ChangeButtonActivity(false);
    bt_continue_best.ChangeButtonActivity(G_Config::GetConfigData().level > 1);
}

void G_GUI_W_MainMenu::OpenWindow()
{
    bt_continue.ChangeButtonActivity(true);
    bt_continue_best.ChangeButtonActivity(G_Config::GetConfigData().level > 1);
    window.SetDrawState(true);
}

void G_GUI_W_MainMenu::CloseWindow()
{
    window.SetDrawState(false);
}

void G_GUI_W_MainMenu::StartNewGame(float)
{
    G_GameManager::pGM->StartNewGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::ContinueGame(float)
{
    G_GameManager::pGM->ContinueGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::ContinueBestGame(float)
{
    G_GameManager::pGM->ContinueBestGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::OpenOptions(float)
{

}

void G_GUI_W_MainMenu::ExitGame(float)
{
    ZC_SWindow::CloseWindow();
}

void G_GUI_W_MainMenu::ButtonFocuseChanged(bool)
{
    
}