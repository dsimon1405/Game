#include "G_GameManager.h"

#include <ZC/ZC__System.h>
#include <System/G_Config.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Events/ZC_Events.h>
#include <Sound/G_GameSounds.h>
#include <Objects/Platforms/G_PlatformWind.h>

G_GameManager::G_GameManager()
    : player(),
    ss_main_theme(G_SN__main_theme, 30.f)
{
    pGM = this;

    ChangeGameLevelsUpdaterState(false);
    // ZC_SWindow::ChangeLevelState(G_UpdaterLevels::G_UL__game_play, false);
    ss_main_theme.upSound->PlayLoop();
}

G_GameManager::~G_GameManager()
{
    ecUpdater.Disconnect();
    G_PlatformWind::FreeParticles();
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
    if (level != 0 && level != 1) G_Config::UpdateGameStats(level, time_total);
    level = 1;
    time_total = {};

    PrepareLevel();
}

void G_GameManager::PauseGame()
{
    assert(game_state == GS_Play || game_state == GS_ContinueWonGame);
    ChangeGamePlayActivityState(false);
    player.ChangeActivityState(false);
    game_state = game_state == GS_Play ? GS_PausePlay : GS_PauseContinueWonGame;
}

void G_GameManager::ContinueGame()
{
    switch (game_state)
    {
    case GS_PausePlay:
    {
        ChangeGamePlayActivityState(true);
            //  if the updater is connected, then the game reports the start of the level, otherwise the game is paused
        gui_start_timer.IsFinished() ? player.ChangeActivityState(true) : player.ChangeCameraState(true);
        game_state = GS_Play;
    } break;
    case GS_PauseContinueWonGame:
    {
        ZC__Updater::ChangeLevelState(G_UL__game_manager, true);
        G_GameSounds::ChangeSoundsPlayState(!move_to_player_on_won_level);
        game_state = GS_ContinueWonGame;
    } break;
    case GS_CreateLevel: PrepareLevel(); break;
    case GS_ContinueWonGame:
    {
        if (!ecUpdater.IsConnected()) ecUpdater.NewConnection(ZC__Updater::Connect({ &G_GameManager::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_manager));
        ZC__Updater::ChangeLevelState(G_UL__game_manager, true);
        G_GameSounds::ChangeSoundsPlayState(!move_to_player_on_won_level);
        move_to_player_on_won_level = true;
            //  calculate cam move dir and speed
        ZC_Camera* pCam = ZC_Camera::GetActiveCamera();
        ZC_Vec3<float> dir_distance = pCam->GetLookOn() - pCam->GetPosition();
        cam_move_dir_normallized = ZC_Vec::Normalize(dir_distance);
        static const float seconds_move_to_player_onprev_lvl = 1.f;
        cam_move_speed = ZC_Vec::Length(dir_distance / seconds_move_to_player_onprev_lvl);
    } break;
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
{
    if (player.GetHealth() == 0.f) return;  //  player already dead
        //  deactivate
    player.ChangeActivityState(false);
    ChangeGamePlayActivityState(false);
        //  open window
    gui.OpenWindow(G_WN__player_win);

    game_state = GS_ContinueWonGame;
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
    G_GameSounds::SetDefaultSate();
        //  prepare
    // map.CreateLevel(21);
    map.CreateLevel(level);
    player.ChangeCameraState(true);
    if (!ecUpdater.IsConnected()) ecUpdater.NewConnection(ZC__Updater::Connect({ &G_GameManager::Callback_Updater, this }, G_UpdaterLevels::G_UL__game_manager));
    ChangeGamePlayActivityState(true);

    game_state = GS_Play;
}

void G_GameManager::Callback_Updater(float time)
{
    if (game_state == GS_ContinueWonGame) ContinueGameUpdate(time);     //  continue won game phase
    else    //  level start timer, then text Level number change scale and pos
    {
            //  at first will calls gui_start_timer.Update() (gui_start_timer.Update and gui_start_timer.IsFinished return same value), if return true start calls gui_level.Update()
        if (gui_start_timer.IsFinished())
        {
            if (gui_level.Update(time)) ecUpdater.Disconnect();     //  process level number up and scale
        }
        else if (gui_start_timer.Update(time))  //  process start timer
        {
            player.SetSoundSetToDefault();
            player.ChangeMoveState(true);
            gui_level_timer.Start();
        }
    }
}

void G_GameManager::ChangeGamePlayActivityState(bool on)
{
    ZC__Updater::ChangeLevelState(G_UL__game_manager, on);
    ZC__Updater::ChangeLevelState(G_UL__camera, on);
    ChangeGameLevelsUpdaterState(on);
    G_GameSounds::ChangeSoundsPlayState(on);
}

void G_GameManager::ChangeGameLevelsUpdaterState(bool on)
{
    ZC__Updater::ChangeLevelState(G_UL__game_play, on);
    ZC__Updater::ChangeLevelState(G_UL__game_particles, on);
}

void G_GameManager::ContinueGameUpdate(float time)
{
    static float must_be_distance_player_to_cam = 0.f;
    static ZC_Vec3<float> must_be_cam_pos;
    static ZC_Camera* pActiveCamera = ZC_Camera::GetActiveCamera();
    
    if (move_to_player_on_won_level)    //  move to player, on won level
    {
        ZC_Vec3<float> new_cam_pos = pActiveCamera->GetPosition() + (cam_move_dir_normallized * (cam_move_speed * time));   //  move cam to player
        if (ZC_Vec::Length(new_cam_pos - pActiveCamera->GetLookOn()) <= player.GetRadius())   //  camera reached sphere (jump inside)
        {
            move_to_player_on_won_level = false;

                //  default state
            player.SetDefaultState();
            gui_start_timer.SetDefaultState();
            gui_level_timer.SetDefaultState();
            gui_level.SetDefaultState(level);
            G_GameSounds::SetDefaultSate();
                //  prepare map
            map.CreateLevel(level);

            ChangeGameLevelsUpdaterState(true);
            G_GameSounds::ChangeSoundsPlayState(true);
            move_to_player_on_won_level = false;
                //  calculate cam move dir and speed
            must_be_cam_pos = pActiveCamera->GetPosition();
            must_be_distance_player_to_cam = ZC_Vec::Length(pActiveCamera->GetLookOn() - must_be_cam_pos);
            ZC_Vec3<float> new_cam_pos = ZC_Vec::MoveByLength(pActiveCamera->GetLookOn(), pActiveCamera->GetPosition(), G_Map::map_radius - 1.f);  //  cam pos near map spheres border
            ZC_Vec3<float> dir_distance = pActiveCamera->GetPosition() - new_cam_pos;
            cam_move_dir_normallized = ZC_Vec::Normalize(dir_distance);
            static const float seconds_move_to_player = 4.f;
            cam_move_speed = ZC_Vec::Length(dir_distance / seconds_move_to_player);
            pActiveCamera->SetPosition(new_cam_pos);
        }
        else pActiveCamera->SetPosition(new_cam_pos);
    }
    else    //  move to player from map sphere border, on new created level
    {
        ZC_Vec3<float> new_cam_pos = pActiveCamera->GetPosition() + (cam_move_dir_normallized * (cam_move_speed * time));   //  move cam to player
        if (ZC_Vec::Length(new_cam_pos - pActiveCamera->GetLookOn()) <= must_be_distance_player_to_cam)
        {
            game_state = GS_Play;
            player.ChangeCameraState(true);
            ZC__Updater::ChangeLevelState(G_UL__camera, true);
            pActiveCamera->SetPosition(must_be_cam_pos);
        }
        else pActiveCamera->SetPosition(new_cam_pos);
    }
}