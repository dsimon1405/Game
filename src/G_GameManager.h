#pragma once

#include <Objects/G_Object.h>

enum G_UpdateLevels
{
    G_UL__update = 0
};

struct G_GameaManager
{
    std::list<ZC_uptr<G_Object>> objs;

    G_GameaManager();
};