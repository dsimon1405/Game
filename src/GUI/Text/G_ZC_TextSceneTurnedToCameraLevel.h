#pragma once

#include <ZC/Objects/Text/ZC_TextSceneTurnedToCamera.h>
#include "G_LanguageDependent.h"

class G_ZC_TextSceneTurnedToCameraLevel : protected G_LanguageDependent, public ZC_TextSceneTurnedToCamera
{
public:
    G_ZC_TextSceneTurnedToCameraLevel();
    
    void VLanguageChanged_LD() override;

        //  virtual from ZC_TextData
    void UpdateLevelText(int _level);

private:
    const std::wstring* pText_level;
    std::wstring level;
};