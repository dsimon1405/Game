#pragma once

#include "G_Player.h"
#include "G_Map.h"
#include <GUI/G_GUI.h>
#include "GUI/G_GUI_GM_W_LevelTimer.h"
#include "GUI/G_GUI_GM_StartTimer.h"
#include "GUI/G_GUI_GM_LevelNumber.h"
#include <Model/G_CubeMap.h>

enum G_GameState
{
    GS_CreateLevel,
        //  active phases
    GS_Play,
    GS_ContinueWonGame,  //  finfished won game, moving camera into sphere then creates level and move camera on new level to player start pos
        //  pauses
    GS_PausePlay,
    GS_PauseContinueWonGame
};

class G_GameManager
{
public:
    static inline G_GameManager* pGM;
    G_GUI gui;

    G_GameManager();
    ~G_GameManager();
    
    G_Time GetLevelTime() const noexcept;
    G_Time GetTotalTime() const noexcept;

    G_GameState GetGameState() const noexcept;

    void StartNewGame();
    void PauseGame();
    void ContinueGame();
    void ContinueBestGame();
    void PlayerDead();
    void PlayerWin();

private:
        //  gm data
    ZC_EC ecUpdater;
        //  game objects
    G_Player player;
    G_Map map;
        //  game state
    G_GameState game_state = GS_CreateLevel;
        //  game data
    int level = 0;
    G_Time time_total;
        //  gui
    G_GUI_GM_StartTimer gui_start_timer;
    G_GUI_GM_W_LevelTimer gui_level_timer;
    G_GUI_GM_LevelNumber gui_level;

    bool move_to_player_on_won_level = true;
    ZC_Vec3<float> cam_move_dir_normallized;
    float cam_move_speed = 0.f;

    G_SystemSound ss_main_theme;

    void PrepareLevel();

    void Callback_Updater(float time);
    void ChangeGamePlayActivityState(bool on);
    void ContinueGameUpdate(float time);
    void ChangeGameLevelsUpdaterState(bool on);
};