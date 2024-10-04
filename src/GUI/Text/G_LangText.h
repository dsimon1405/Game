#pragma once

#include "G_Language.h"

#include <string>

struct G_LangText
{
    G_Language lang = G_L_Russian;
    std::wstring text;

    bool operator == (G_Language _lang);
};