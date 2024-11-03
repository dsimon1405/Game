#include "G_GUI_W_MainMenu.h"

#include <GamePlay/G_GameManager.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>

G_GUI_W_MainMenu::G_GUI_W_MainMenu()
    : window(ZC_WOIData(500.f, 430.f, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center),
    ZC_GUI_WF__NeedDraw),
    // 0),
    text_game(ZC_GUI_TextAlignment::Center, 0, { G_LangText{ .lang = G_L_Russian, .text = L"С-Ф-Е-Р-У-М" }, { .lang = G_L_English, .text = L"S-P-H-E-R-U-M" } }),
    bt_start({ &G_GUI_W_MainMenu::StartNewGame, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Новая игра" }, { .lang = G_L_English, .text = L"New game" } }),
    bt_continue({ &G_GUI_W_MainMenu::ContinueGame, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Продолжить игру" }, { .lang = G_L_English, .text = L"Continue game" } }),
    bt_continue_best({ &G_GUI_W_MainMenu::ContinueBestGame, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Загрузить сохранeние" }, { .lang = G_L_English, .text = L"Load saved" } }),
    bt_options({ &G_GUI_W_MainMenu::OpenOptions, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Настройки" }, { .lang = G_L_English, .text = L"Options" } }),
    bt_exit({ &G_GUI_W_MainMenu::ExitGame, this }, {}, true,
        { G_LangText{ .lang = G_L_Russian, .text = L"Покинуть игру" }, { .lang = G_L_English, .text = L"Leave the game" } })
{
    const float distance_y = 20.f;
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { text_game.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { bt_start.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { bt_continue.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { bt_continue_best.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { bt_options.GetObj() }));
    window.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Center, distance_y, 0.f), { bt_exit.GetObj() }));

    bt_continue.ChangeButtonActivity(false);
    bt_continue_best.ChangeButtonActivity(G_Config::GetConfigData().level > 1);
}

void G_GUI_W_MainMenu::OpenWindow()
{
    bt_continue.ChangeButtonActivity(game_was_launched);
    bt_continue_best.ChangeButtonActivity(G_Config::GetConfigData().level > 1);
    window.SetDrawState(true);
}

void G_GUI_W_MainMenu::CloseWindow()
{
    window.SetDrawState(false);
}

void G_GUI_W_MainMenu::StartNewGame(float)
{
    game_was_launched = true;
    G_GameManager::pGM->StartNewGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::ContinueGame(float)
{
    game_was_launched = true;
    G_GameManager::pGM->ContinueGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::ContinueBestGame(float)
{
    game_was_launched = true;
    G_GameManager::pGM->ContinueBestGame();
    G_GameManager::pGM->gui.CloseCurrentWindow();
}

void G_GUI_W_MainMenu::OpenOptions(float)
{
    G_GameManager::pGM->gui.OpenWindow(G_WN__options);
}

void G_GUI_W_MainMenu::ExitGame(float)
{
    ZC_SWindow::BreakMainCycle();
}