#pragma once

#include "G_Player.h"
#include "G_Map.h"
#include "GUI/G_GUI_GM_W_LevelTimer.h"
#include <GUI/G_GUI.h>
#include "System/G_Time.h"

enum G_GameState
{
    GS_Default,
    GS_Play,
    GS_Pause,
    GS_RestartLevel,
    GS_NextLevel
};

class G_GameManager
{
public:
    static inline G_GameManager* pGM;
    G_GUI gui;

    G_GameManager();
    ~G_GameManager();
    
    G_Time GetLevelTime();
    G_Time GetTotalTime();

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
    G_GameState game_state = GS_Default;
        //  game data
    int level = 1;
    G_Time time_cur_lvl;
    G_Time time_total;
    float cur_time = 0.f;
        //  gui
    G_GUI_GM_W_LevelTimer gui_w_time;

    void Callback_Updater(float time);

    void SetCurrentLevelTimeDefault();
    void ChangeGamePlayActivityState(bool avitve);
};