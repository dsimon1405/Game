#include "G_DrawerCreator.h"

#include "G_RenderLevel.h"
#include <ZC/Video/OpenGL/Renderer/ZC_DrawerFL.h>

bool G_DrawerCreator::Init()
{
    ZC_Drawer::SetUsersFunction_GetRendererLevelDrawer({ &(G_DrawerCreator::GetRendererLevelDrawer) });
    return true;
}

ZC_uptr<ZC_Drawer> G_DrawerCreator::GetRendererLevelDrawer(ZC_DrawerLevel lvl)
{
    switch (lvl)
    {
    case G_DL_CubeMap: return
        { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, false, false, false, {}) };
    case G_DL_AlphaBlending_ParticleStar: return
        { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, false, false, false, ZC_GLBlend(GL_SRC_ALPHA, GL_ONE)) };
    case G_DL_AlphaBlending_ParticlesWind: return
        { new ZC_DrawerFL<const ZC_ShProg*, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, true, false, false, ZC_GLBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)) };
    case G_DL_AlphaBlending_PlatformDisapear: return
        { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, true, false, false, ZC_GLBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)) };
    default: assert(false); return nullptr;
    }
}


    // G_DL_CubeMap = ZC_DL_Drawing - 50,
    // G_DL_AlphaBlending_ParticleStar = G_DL_CubeMap + 1,
    // G_DL_AlphaBlending_ParticlesWind = ZC_DrawerLevels::TextScene + 1,
    // G_DL_AlphaBlending_PlatformDisapear = G_DL_AlphaBlending_PlatformDisapear + 1,