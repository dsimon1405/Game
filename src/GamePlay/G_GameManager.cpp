#include "G_GameManager.h"

#include <Objects/G_OP_MarbleSphere.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_FontData.h>
#include <ZC/Events/ZC_Events.h>

G_GameManager::G_GameManager()
    : player((new G_OP_MarbleSphere(true))),
    tw_start_counter
        {
            ZC_TextWindow(G_FontData::fd_Arial_200, "3", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, "2", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, "1", ZC_TA_Left, 0.f, 0.f, ZC_WOIF__X_Center | ZC_WOIF__Y_Center, false)
        },
    tstc_level(G_FontData::fd_Arial_200, "", ZC_TA_Left, { 0.f, 0.f, tstc_level_start_Z }, true),
    tstc_level_start_scale(tstc_level.GetScale())
{
    pGM = this;
    tw_start_counter[0].SetColorUChar(255, 0, 0);
    tw_start_counter[1].SetColorUChar(255, 255, 0);
    tw_start_counter[2].SetColorUChar(0, 255, 0);

    tstc_level.SetColorUChar(200, 200, 200);

    ZC_SWindow::ChangeUpdaterLevelState(G_UpdaterLevels::G_UL__game_play, false);
}

G_GameManager::~G_GameManager()
{
    ecUpdater.Disconnect();
}

G_Time G_GameManager::GetLevelTime() const noexcept
{
    return w_level_timer.GetTime();
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
    G_Config::UpdateGameStats(level, time_total);
    level = 1;
    time_total = {};

    PrepareLevel();
}

void G_GameManager::PauseGame()
{
    assert(game_state == GS_Play);
    ChangeGamePlayUpdaterState(false);
    player.ChangeActivityState(false);
    game_state = GS_Pause;
}

void G_GameManager::ContinueGame()
{
    switch (game_state)
    {
    case GS_Pause:
    {
        ChangeGamePlayUpdaterState(true);
        ecUpdater.IsConnected() ? player.ChangeCameraState(true) : player.ChangeActivityState(true);
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
    ChangeGamePlayUpdaterState(false);
        //  open window
    gui.OpenWindow(G_WN__restart_level);

    game_state = GS_CreateLevel;
}

void G_GameManager::PlayerWin()
{       //  deactivate
    player.ChangeActivityState(false);
    ChangeGamePlayUpdaterState(false);
        //  open window
    gui.OpenWindow(G_WN__player_win);

    game_state = GS_CreateLevel;
        //  save changes
    time_total.PlusSeconds(w_level_timer.GetTime().GetInSeconds());
    ++level;
    G_Config::UpdateGameStats(level, time_total);
}

void G_GameManager::PrepareLevel()
{
        //  default
    player.SetDefaultState();
    w_level_timer.SetDefaultState();
    cur_time = 0.f;
    tw_index = 0;
        //  preapare
    map.CreateLevel(level);
    player.ChangeCameraState(true);
    ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_GameManager::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_play));
    ChangeGamePlayUpdaterState(true);
        //  enable first, other off
    tw_start_counter[0].NeedDraw(true);
    tw_start_counter[1].NeedDraw(false);
    tw_start_counter[2].NeedDraw(false);

    tstc_level.SetText("Level " + std::to_string(level));
    tstc_level.SetPosition({ 0.f, 0.f, tstc_level_start_Z });
    tstc_level.SetScale(tstc_level_start_scale);

    game_state = GS_Play;
}

void G_GameManager::Callback_Updater(float time)
{
    cur_time += time;

        //  when game started (look lower) make scale and move up - level text
    if (tw_index == 3)
    {
        static const float seconds_txts_prepare = 2.f;
        static const float tstc_level_length_to_end_Z = 10.f - tstc_level_start_Z;      //  10 is end
        static const float tstc_level_range_to_end_scale = 0.05f - tstc_level_start_scale;  //  0.05 is end
        
        if (cur_time >= seconds_txts_prepare) ecUpdater.Disconnect();
        else
        {
            float time_coef = cur_time / seconds_txts_prepare;
            tstc_level.SetPosition({ 0.f, 0.f, tstc_level_start_Z + (tstc_level_length_to_end_Z * time_coef) });
            tstc_level.SetScale(tstc_level_start_scale + (tstc_level_range_to_end_scale * time_coef));
        }
        return;     //  don't go furthe!
    }

        //  show counter one by one, then start level
    static const float start_second = 1.f;
    if (cur_time >= start_second)
    {
        tw_start_counter[tw_index].NeedDraw(false);
        cur_time = 0.f;
        ++tw_index;
        if (tw_index == 3)
        {
            player.ChangeMoveState(true);
            w_level_timer.Start();
            return;
        }
        else tw_start_counter[tw_index].NeedDraw(true);
    }
    tw_start_counter[tw_index].SetAlpha(1.f - (cur_time / start_second));
}

void G_GameManager::ChangeGamePlayUpdaterState(bool on)
{
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__camera, on);
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, on);
}