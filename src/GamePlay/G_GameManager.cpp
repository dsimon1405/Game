#include "G_GameManager.h"

#include <Objects/G_OP_MarbleSphere.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>
#include <System/G_UpdaterLevels.h>

G_GameManager::G_GameManager()
    : player((new G_OP_MarbleSphere()))
{
    pGM = this;
}

G_GameManager::~G_GameManager()
{
    ecUpdater.Disconnect();
}

G_Time G_GameManager::GetLevelTime() const noexcept
{
    return time_cur_lvl;
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
    if (game_state != GS_Default)
    {
            //  config file data
        G_Config::UpdateGameStats(level, time_total);
            //  level
        level = 1;
            //  text time
        SetCurrentLevelTimeDefault();
        time_total = {};
            //  player
        player.SetDefaultState();
    }
    map.CreateLevel(level);
    ChangeGamePlayActivityState(true);
    game_state = GS_Play;
    cur_time = 0.f;
}

void G_GameManager::PauseGame()
{
    assert(game_state != GS_Play);
    ChangeGamePlayActivityState(false);
    game_state = GS_Pause;
}

void G_GameManager::ContinueGame()
{
    assert(game_state != GS_Pause);
    switch (game_state)
    {
    case GS_Pause: break;
    case GS_RestartLevel:
    {
        player.SetDefaultState();
        map.CreateLevel(level);
        SetCurrentLevelTimeDefault();
        cur_time = 0.f;
    } break;
    case GS_NextLevel:
    {
        player.SetDefaultState();
        map.CreateLevel(level);
        SetCurrentLevelTimeDefault();
        cur_time = 0.f;
    } break;
    default: assert(false);
    }
    ChangeGamePlayActivityState(true);
    game_state = GS_Play;
}

void G_GameManager::ContinueBestGame()
{
    const G_ConfigData& conf_data = G_Config::GetConfigData();
    level = conf_data.level;
    time_total = conf_data.time;

    if (game_state != GS_Default)
    {
        player.SetDefaultState();
        SetCurrentLevelTimeDefault();
    }

    map.CreateLevel(level);
    ChangeGamePlayActivityState(true);
    game_state = GS_Play;

    cur_time = 0.f;
}

void G_GameManager::PlayerDead()
{
    ChangeGamePlayActivityState(false);
    gui.OpenWindow(G_WN__restart_level);
    game_state = GS_RestartLevel;
}

void G_GameManager::PlayerWin()
{
    ChangeGamePlayActivityState(false);
    gui.OpenWindow(G_WN__player_win);
    game_state = GS_NextLevel;

    time_total.PlusSeconds(time_cur_lvl.GetInSeconds());
    ++level;
    G_Config::UpdateGameStats(level, time_total);
}


// void G_GameManager::ChangeEventsAndUpdaterState(bool make_active)
// {
//     if (!(pGM->isGameOn) || pGM->isEventAndUpdatesOn == make_active) return;
//     pGM->isEventAndUpdatesOn = make_active;
//     // for (auto& upObj : pGM->objs) upObj->SetEventsState(pGM->isEventAndUpdatesOn);
//     ZC_SWindow::ChangeUpdaterState(pGM->isEventAndUpdatesOn);
//     ZC_SWindow::HideCursor();
// }

void G_GameManager::Callback_Updater(float time)
{
    static const float one_second = 1.f;

    cur_time += time;
    if (cur_time >= one_second)
    {
        G_Time prev_time = time_cur_lvl;

        float seconds = 0.f;    //  whole part
        cur_time = std::modf(cur_time, &seconds);
        time_cur_lvl.PlusSeconds(seconds);    //  whole part of the time (seconds) add to time_cur_lvl, fractional set in cur_time
            //  update text
        if (prev_time.hours != time_cur_lvl.hours)
            gui_w_time.UpdateHours(time_cur_lvl.hours > 9 ? std::to_wstring(time_cur_lvl.hours) : L"0" + std::to_wstring(time_cur_lvl.hours));
        if (prev_time.minutes != time_cur_lvl.minutes)
            gui_w_time.UpdateMinutes(time_cur_lvl.minutes > 9 ? std::to_wstring(time_cur_lvl.minutes) : L"0" + std::to_wstring(time_cur_lvl.minutes));
        if (prev_time.seconds != time_cur_lvl.seconds)
            gui_w_time.UpdateSeconds(time_cur_lvl.seconds > 9 ? std::to_wstring(time_cur_lvl.seconds) : L"0" + std::to_wstring(time_cur_lvl.seconds));
    }
}

void G_GameManager::SetCurrentLevelTimeDefault()
{
        //  text time
    if (time_cur_lvl.hours != 0) gui_w_time.UpdateHours(L"00");
    if (time_cur_lvl.minutes != 0) gui_w_time.UpdateMinutes(L"00");
    if (time_cur_lvl.seconds != 0) gui_w_time.UpdateSeconds(L"00");
        //  timer
    time_cur_lvl = {};
}

void G_GameManager::ChangeGamePlayActivityState(bool avitve)
{
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, avitve);
    player.SetEventsState(avitve);
    avitve ? ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_GameManager::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play))
        : ecUpdater.Disconnect();
}