#pragma once

#include "G_Player.h"
#include "G_Map.h"
#include "GUI/G_GUI_GM_W_LevelTimer.h"
#include <GUI/G_GUI.h>
#include "System/G_Time.h"
#include <ZC/Objects/Text/ZC_TextSceneTurnedToCamera.h>

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
    ZC_EC ecChangeCamPos;
        //  game objects
    G_Player player;
    G_Map map;
        //  game state
    G_GameState game_state = GS_CreateLevel;
        //  game data
    int level = 0;
    G_Time time_total;
        //  gui (timer)
    G_GUI_GM_W_LevelTimer w_level_timer;
        //  updater data
    ZC_TextWindow tw_start_counter[3];
    float cur_time = 0.f;
    unsigned char tw_index = 0;
    ZC_TextSceneTurnedToCamera tstc_level;

    void PrepareLevel();

    void Callback_Updater(float time);
    void Callback_ChangeCamPos(const ZC_Vec3<float>& cam_pos);

    // void SetCurrentLevelTimeDefault();
    // void ChangeGamePlayActivityState(bool active);
};