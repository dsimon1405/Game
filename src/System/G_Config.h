#pragma once

#include <GUI/Text/G_Language.h>
#include "G_Time.h"

#include <string>

struct G_ConfigData
{
    char name[4] { 'G', 'A', 'M', 'E' };
    G_Language language = G_L_Russian;
    float volume = 80.f;
    unsigned short level = 1;
    G_Time time;
};

struct G_Config
{
    G_Config();
    ~G_Config();

    static const G_ConfigData& GetConfigData();
    static void Update_language(G_Language language);
    static void Update_volume(float volume);
    static void UpdateGameStats(unsigned char level, G_Time time);

private:
    static inline G_Config* pConfig;
    G_ConfigData config_data;
    std::string config_path;
    bool config_data_changed = false;

    bool ConstCharEqual(const char* first, char* second) noexcept;
};