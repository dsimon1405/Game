#include "G_Config.h"

#include <ZC/File/ZC_File.h>
#include <GUI/Text/G_LanguageDependent.h>
#include <ZC/Video/ZC_SWindow.h>
#include <Sound/G_GameSound.h>
#include <Sound/G_SystemSound.h>

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

void G_Config::CreateGUI()
{
    upW_save_level = new G_GUI_W_ConfigSaveLevel();
}

const G_ConfigData& G_Config::GetConfigData()
{
    return pConfig->config_data;
}

void G_Config::Update_language(unsigned int language)
{
    pConfig->config_data.language = static_cast<G_Language>(language);
    G_LanguageDependent::LanguageChanged();     //  update all texts with different languages
    pConfig->config_data_changed = true;
}

void G_Config::Update_full_screen(bool full_screen)
{
    pConfig->config_data.fuull_screen = full_screen;
    ZC_SWindow::SetFullScreen(full_screen);
    pConfig->config_data_changed = true;
}

void G_Config::Update_volume(int volume)
{
    pConfig->config_data.volume_coef = float(volume) / 100.f;
    G_GameSound::UpdateSoundsVolume();
    G_SystemSound::UpdateSoundsVolume();
    pConfig->config_data_changed = true;
}

void G_Config::UpdateGameStats(int level, G_Time time)
{
    auto lamb_save = [level, &time]()
    {
        pConfig->config_data.level = level;
        pConfig->config_data.time = time;
        pConfig->config_data_changed = true;

        pConfig->upW_save_level->ShowSavedData(level, time);
    };

    if (pConfig->config_data.level < level) lamb_save();
    else if (pConfig->config_data.time.GetInSeconds() > time.GetInSeconds()) lamb_save();
}

bool G_Config::ConstCharEqual(const char* first, char* second) noexcept
{
    for (ulong i = 0; ; ++i)
    {
        if (first[i] == '\0') return true;
        if (first[i] != second[i]) return false;
    }
}