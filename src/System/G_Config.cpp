#include "G_Config.h"

#include <ZC/File/ZC_File.h>
#include <GUI/Text/G_LanguageDependent.h>

#include <fstream>

G_Config::G_Config()
    : config_path(ZC_FSPath(ZC_assetsDirPath).append("config.g").string())
{
    pConfig = this;

        //  read config file
    std::ifstream in_file(config_path.c_str(), std::ios::binary);
    if (!(in_file.is_open()))  //  can't open file
    {
        config_data_changed = true;
        return;
    }

    in_file.read(reinterpret_cast<char*>(&config_data), sizeof(config_data));
    
    const char must_be_name[] { 'G', 'A', 'M', 'E' };
    if (!ConstCharEqual(must_be_name, config_data.name))
        config_data = {};
}

G_Config::~G_Config()
{
    if (!config_data_changed) return;

    std::ofstream of_stream(config_path.c_str(), std::ios::binary);
    if (!(of_stream.is_open())) return;

    of_stream.write(reinterpret_cast<char*>(&config_data), sizeof(config_data));

    of_stream.close();
}

const G_ConfigData& G_Config::GetConfigData()
{
    return pConfig->config_data;
}

void G_Config::Update_language(G_Language language)
{
    pConfig->config_data.language = language;
    G_LanguageDependent::LanguageChanged();     //  update all texts with different languages
    pConfig->config_data_changed = true;
}

void G_Config::Update_volume(float volume)
{
    pConfig->config_data.volume = volume;
    pConfig->config_data_changed = true;
}

void G_Config::UpdateGameStats(unsigned char level, G_Time time)
{
    if (pConfig->config_data.level > level && pConfig->config_data.time.hours < time.hours && pConfig->config_data.time.minutes < time.minutes
        && pConfig->config_data.time.seconds < time.seconds) return;
    pConfig->config_data.level = level;
    pConfig->config_data.time = time;
    pConfig->config_data_changed = true;
}

bool G_Config::ConstCharEqual(const char* first, char* second) noexcept
{
    for (ulong i = 0; ; ++i)
    {
        if (first[i] == '\0') return true;
        if (first[i] != second[i]) return false;
    }
}