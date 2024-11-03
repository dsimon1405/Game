#include "G_ZC_TextSceneTurnedToCameraLevel.h"

#include <System/G_FontData.h>

G_ZC_TextSceneTurnedToCameraLevel::G_ZC_TextSceneTurnedToCameraLevel()
    : G_LanguageDependent({ G_LangText{ .lang = G_L_Russian, .text = L"Уровень " }, { G_L_English, L"Level " } }),
    ZC_TextSceneTurnedToCamera(G_FontData::fd_Arial_200, L"0", ZC_TA_Left, {}, true),
    pText_level(&(this->GetCurrentLanguageText(this->lang_texts)))
{
    this->SetColorUChar(200, 200, 200);
}
    
void G_ZC_TextSceneTurnedToCameraLevel::VLanguageChanged_LD()
{
    pText_level = &(this->GetCurrentLanguageText(this->lang_texts));
    this->SetText(*pText_level + level);
}

void G_ZC_TextSceneTurnedToCameraLevel::UpdateLevelText(int _level)
{
    level = std::to_wstring(_level);
    this->SetText(*pText_level + level);
}