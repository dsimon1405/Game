#include "G_GameManager.h"

#include <Objects/G_OP_MarbleSphere.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Events/ZC_Events.h>
#include <Sound/G_GameSound.h>

G_GameManager::G_GameManager()
    : player((new G_OP_MarbleSphere(true)))
{
    pGM = this;

    ZC_SWindow::ChangeUpdaterLevelState(G_UpdaterLevels::G_UL__game_play, false);
}

G_GameManager::~G_GameManager()
{
    ecUpdater.Disconnect();
}

G_Time G_GameManager::GetLevelTime() const noexcept
{
    return gui_level_timer.GetTime();
}

G_Time G_GameManager::GetTotalTime() const noexcept
{
    return time_total;
}

G_GameState G_GameManager::GetGameState() const noexcept
{
    return game_state;
}

void G_GameManager::StartNewGame()
{
    if (level != 0) G_Config::UpdateGameStats(level, time_total);
    level = 1;
    time_total = {};

    PrepareLevel();
}

void G_GameManager::PauseGame()
{
    assert(game_state == GS_Play);
    ChangeGamePlayActivityState(false);
    player.ChangeActivityState(false);
    game_state = GS_Pause;
}

void G_GameManager::ContinueGame()
{
    switch (game_state)
    {
    case GS_Pause:
    {
        ChangeGamePlayActivityState(true);
            //  if the updater is connected, then the game reports the start of the level, otherwise the game is paused
        gui_start_timer.IsFinished() ? player.ChangeActivityState(true) : player.ChangeCameraState(true);
        game_state = GS_Play;
    } break;
    case GS_CreateLevel: PrepareLevel(); break;
    default: assert(false);
    }
}

void G_GameManager::ContinueBestGame()
{
    const G_ConfigData& conf_data = G_Config::GetConfigData();
    level = conf_data.level;
    time_total = conf_data.time;
    
    PrepareLevel();
}

void G_GameManager::PlayerDead()
{       //  deactivate
    player.ChangeActivityState(false);
    ChangeGamePlayActivityState(false);
        //  open window
    gui.OpenWindow(G_WN__restart_level);

    game_state = GS_CreateLevel;
}

void G_GameManager::PlayerWin()
{       //  deactivate
    player.ChangeActivityState(false);
    ChangeGamePlayActivityState(false);
        //  open window
    gui.OpenWindow(G_WN__player_win);

    game_state = GS_CreateLevel;
        //  save changes
    time_total.PlusSeconds(gui_level_timer.GetTime().GetInSeconds());
    ++level;
    G_Config::UpdateGameStats(level, time_total);
}

void G_GameManager::PrepareLevel()
{
        //  default
    player.SetDefaultState();
    gui_start_timer.SetDefaultState();
    gui_level_timer.SetDefaultState();
    gui_level.SetDefaultState(level);
        //  preapare
    map.CreateLevel(level);
    player.ChangeCameraState(true);
    ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_GameManager::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play));
    ChangeGamePlayActivityState(true);

    game_state = GS_Play;
}

void G_GameManager::Callback_Updater(float time)
{
        //  at first will calls gui_start_timer.Update() (gui_start_timer.Update and gui_start_timer.IsFinished return same value), if return true start calls gui_level.Update()

    if (gui_start_timer.IsFinished())
    {
        if (gui_level.Update(time)) ecUpdater.Disconnect();     //  process level number up and scale
    }
    else if (gui_start_timer.Update(time))  //  process start timer
    {
        player.ChangeMoveState(true);
        gui_level_timer.Start();
    }
}

void G_GameManager::ChangeGamePlayActivityState(bool on)
{
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__camera, on);
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, on);
    G_GameSound::ChangeSoundsPlayState(on);
}