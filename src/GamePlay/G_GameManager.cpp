#include "G_GameManager.h"

#include <Objects/G_OP_MarbleSphere.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_Config.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_FontData.h>
#include <ZC/Events/ZC_Events.h>
ZC_Vec3<float> pos{0,0,4};
G_GameManager::G_GameManager()
    : player((new G_OP_MarbleSphere())),
    tw_start_counter
        {
            ZC_TextWindow(G_FontData::fd_Arial_200, "3", ZC_TA_Left, 0.f, 0.1f, ZC_WOIF__X_Center | ZC_WOIF__Y_Top_Percent, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, "2", ZC_TA_Left, 0.f, 0.1f, ZC_WOIF__X_Center | ZC_WOIF__Y_Top_Percent, false),
            ZC_TextWindow(G_FontData::fd_Arial_200, "1", ZC_TA_Left, 0.f, 0.1f, ZC_WOIF__X_Center | ZC_WOIF__Y_Top_Percent, false)
        },
    tstc_level(G_FontData::fd_Arial_200, "", ZC_TA_Left, {0, 0, 10}, true, 0.05f)
{
    pGM = this;
    tw_start_counter[0].SetColorUChar(255, 0, 0);
    tw_start_counter[1].SetColorUChar(255, 255, 0);
    tw_start_counter[2].SetColorUChar(0, 255, 0);

    tstc_level.SetColorUChar(200, 200, 200);

    ecChangeCamPos = ZC_Events::ConnectActiveCameraChangePosition({ &G_GameManager::Callback_ChangeCamPos, this });
}

G_GameManager::~G_GameManager()
{
    ecUpdater.Disconnect();
    ecChangeCamPos.Disconnect();
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
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, false);
    player.ChangeActivityState(false);
    game_state = GS_Pause;
}

void G_GameManager::ContinueGame()
{
    switch (game_state)
    {
    case GS_Pause:
    {
        ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, true);
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
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, false);
        //  open window
    gui.OpenWindow(G_WN__restart_level);

    game_state = GS_CreateLevel;
}

void G_GameManager::PlayerWin()
{       //  deactivate
    player.ChangeActivityState(false);
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, false);
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
    ZC_SWindow::ChangeUpdaterLevelState(G_UL__game_play, true);
        //  enable first, other off
    tw_start_counter[0].NeedDraw(true);
    tw_start_counter[1].NeedDraw(false);
    tw_start_counter[2].NeedDraw(false);

    tstc_level.SetText("Level " + std::to_string(level));

    game_state = GS_Play;
}

void G_GameManager::Callback_Updater(float time)
{
    static float start_second = 1.f;

    cur_time += time;
    if (cur_time >= start_second)
    {
        tw_start_counter[tw_index].NeedDraw(false);
        cur_time = 0.f;
        ++tw_index;
        if (tw_index == 3)
        {
            player.ChangeMoveState(true);
            w_level_timer.Start();
            ecUpdater.Disconnect();
            return;
        }
        else tw_start_counter[tw_index].NeedDraw(true);
    }
    tw_start_counter[tw_index].SetAlpha(1.f - (cur_time / start_second));

    if (tw_index == 2)
    {
        // static const float must_be_scale = 0.000001f;
        // tstc_level.SetScale(must_be_scale * cur_time / start_second);
    }
}

void G_GameManager::Callback_ChangeCamPos(const ZC_Vec3<float>& cam_pos)
{
    // float length = ZC_Vec::Length(cam_pos - pos);
    // tstc_level.SetScale(length / 1005.f);
    // tstc_level.SetPosition({ 0.f, 0.f, pos[2] + length / 100.f });
}