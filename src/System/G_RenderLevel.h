#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_RenderLevel.h>

enum G_RenderLevel
{
};

enum G_DrawerLevel
{
    G_DL_CubeMap = ZC_DL_Drawing - 1,
    G_DL_AlphaBlending_PlatformDisapear = ZC_DrawerLevels::TextScene + 1,
    G_DL_AlphaBlending_ParticlesWind = G_DL_AlphaBlending_PlatformDisapear + 1
};