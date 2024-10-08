#pragma once

#include "G_Player.h"
#include "G_Map.h"
#include <GUI/G_GUI.h>
#include "GUI/G_GUI_GM_W_LevelTimer.h"
#include "GUI/G_GUI_GM_StartTimer.h"
#include "GUI/G_GUI_GM_LevelNumber.h"

enum G_GameState
{
    GS_CreateLevel,
    GS_Play,
    GS_Pause,
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

    void PrepareLevel();

    void Callback_Updater(float time);
    void ChangeGamePlayActivityState(bool on);
};