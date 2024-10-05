#include "G_GUI.h"

#include <ZC/Video/ZC_SWindow.h>
#include <ZC/Events/ZC_Events.h>

G_GUI::G_GUI()
{
    ZC_Events::ConnectWindowCloseButton({ &G_GUI::CloseSWindowButtonClicked, this });
    ZC_Events::ConnectButtonClick(ZC_ButtonID::K_ESCAPE, nullptr, { &G_GUI::EscapeeClicked, this });
}

void G_GUI::OpenWindow(G_WindowName win)
{
    assert(win != opened_window);

    ChangeOpenedWindowDrawState(false);
    if (opened_window == G_WN__none)
    {
        // ZC_SWindow::ShowCursor();
        // ZC_SWindow::UnlimitCursor();
    }
    opened_window = win;
    ChangeOpenedWindowDrawState(true);
}

void G_GUI::CloseCurrentWindow()
{
    ChangeOpenedWindowDrawState(false);
    // ZC_SWindow::HideCursor();
    // ZC_SWindow::LimitCursor();
    opened_window = G_WN__none;
}

void G_GUI::CloseSWindowButtonClicked()
{
    // if (this->IsDrawing()) return;
    // this->SetDrawState(true);
    // G_GameManager::ChangeEventsAndUpdaterState(false);
    // ZC_SWindow::ShowCursor();
}

void G_GUI::EscapeeClicked(ZC_ButtonID,float)
{
    // EscapeeClicked();
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