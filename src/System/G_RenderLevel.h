#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_RenderLevel.h>

enum G_RenderLevel
{
};

enum G_DrawerLevel
{
    G_DL_CubeMap = ZC_DL_Drawing - 50,
    G_DL_AlphaBlending_ParticleStar = G_DL_CubeMap + 1,
    G_DL_AlphaBlending_ParticlesWind = ZC_DrawerLevels::TextScene + 1,
    G_DL_AlphaBlending_PlatformDisappear = G_DL_AlphaBlending_ParticlesWind + 1,
};