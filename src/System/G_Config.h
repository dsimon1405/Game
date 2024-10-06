#pragma once

#include <GUI/Text/G_Language.h>
#include "G_Time.h"
#include <GUI/Config/G_GUI_W_ConfigSaveLevel.h>

#include <string>

struct G_ConfigData
{
    char name[4] { 'G', 'A', 'M', 'E' };
    G_Language language = G_L_Russian;
    bool fuull_screen = true;
    int volume = 80;
    int level = 1;
    G_Time time;
};

struct G_Config
{
    G_Config();
    ~G_Config();

    void CreateGUI();

    static const G_ConfigData& GetConfigData();
    static void Update_language(unsigned int language);
    static void Update_full_screen(bool full_screen);
    static void Update_volume(int volume);
    static void UpdateGameStats(int level, G_Time time);

private:
    static inline G_Config* pConfig;
    G_ConfigData config_data;
    std::string config_path;
    bool config_data_changed = false;
    ZC_uptr<G_GUI_W_ConfigSaveLevel> upW_save_level;

    bool ConstCharEqual(const char* first, char* second) noexcept;
};