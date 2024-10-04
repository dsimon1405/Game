#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_Drawer.h>

struct G_DrawerCreator
{
private:
    static bool Init();

    static inline bool init = Init();

    static ZC_uptr<ZC_Drawer> GetRendererLevelDrawer(ZC_DrawerLevel lvl);
};