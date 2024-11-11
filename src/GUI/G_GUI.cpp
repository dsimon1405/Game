#include "G_GUI.h"

#include <ZC/ZC__System.h>
#include <ZC/Events/ZC_Events.h>
#include <GamePlay/G_GameManager.h>

G_GUI::G_GUI()
{
    ZC_Events::ConnectWindowCloseButton({ &G_GUI::CloseSWindowButtonClicked, this });
    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_ESCAPE, nullptr, { &G_GUI::EscapeeClicked, this });
}

void G_GUI::OpenWindow(G_WindowName win)
{
    assert(win != opened_window);

    if (opened_window == G_WN__main_menu) try_close_game = false;

    ChangeOpenedWindowDrawState(false);
    if (opened_window == G_WN__none)
    {
        ZC__Window::ShowCursor();
        ZC__Window::UnlimitCursor();
    }
    opened_window = win;
    ChangeOpenedWindowDrawState(true);

    ss_open_window.upSound->Play();
}

void G_GUI::CloseCurrentWindow()
{
    if (opened_window == G_WN__main_menu) try_close_game = false;
    ChangeOpenedWindowDrawState(false);
    ZC__Window::HideCursor();
    ZC__Window::LimitCursor();
    opened_window = G_WN__none;

    ss_open_window.upSound->Play();
}

void G_GUI::CloseSWindowButtonClicked()
{
    if (try_close_game) return;
    try_close_game = true;
    GoBackToMainMenu();

    ss_open_window.upSound->Play();
}

void G_GUI::EscapeeClicked(ZC_ButtonID,float)
{
    GoBackToMainMenu();
    
    ss_open_window.upSound->Play();
}

void G_GUI::GoBackToMainMenu()
{
    switch (opened_window)
    {
    case G_WN__none:
    {
        G_GameManager::pGM->PauseGame();
        OpenWindow(G_WN__main_menu);
    } break;
    case G_WN__main_menu:
    {
        G_GameState game_state = G_GameManager::pGM->GetGameState();
        if (game_state == GS_PausePlay || game_state == GS_PauseContinueWonGame)
        {
            G_GameManager::pGM->ContinueGame();
            CloseCurrentWindow();
        }
    } break;
    case G_WN__options: OpenWindow(G_WN__main_menu); break;
    case G_WN__restart_level: OpenWindow(G_WN__main_menu); break;
    case G_WN__player_win: OpenWindow(G_WN__main_menu); break;
    }
}

void G_GUI::ChangeOpenedWindowDrawState(bool need_draw)
{
    switch (opened_window)
    {
    case G_WN__none: break;
    case G_WN__main_menu: need_draw ? w_main_menu.OpenWindow() : w_main_menu.CloseWindow(); break;
    case G_WN__options: w_options.SetDrawState(need_draw); break;
    case G_WN__restart_level: w_restart_level.SetDrawState(need_draw); break;
    case G_WN__player_win: need_draw ? w_player_win.OpenWindow() : w_player_win.CloseWindow(); break;
    }
}