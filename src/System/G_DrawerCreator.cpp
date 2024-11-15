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
    case G_DL_CubeMap: return { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>
            (0, ZC_GLDepth(false), ZC_GLStencil(false), ZC_GLBlend(false), ZC_GLCullFace(false)) };
    case G_DL_AlphaBlending_ParticleStar: return { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>
            (0, ZC_GLDepth(false), ZC_GLStencil(false), ZC_GLBlend(true, GL_SRC_ALPHA, GL_ONE), ZC_GLCullFace(false)) };
    // case G_DL_AlphaBlending_ParticleFlame: return { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>
    //         (0, ZC_GLDepth(false), ZC_GLStencil(false), ZC_GLBlend(true, GL_SRC_ALPHA, GL_ONE), ZC_GLCullFace(false)) };
    case G_DL_AlphaBlending_ParticlesWind: return { new ZC_DrawerFL<const ZC_ShProg*, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, ZC_GLDepth(true), ZC_GLStencil(false),
        ZC_GLBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), ZC_GLCullFace(true))};  //  cull face no metter for points, don't disable here caurse neighboring levels use face cull 
    case G_DL_AlphaBlending_PlatformDisappear: return { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>
            (0, ZC_GLDepth(true, GL_FALSE), ZC_GLStencil(false), ZC_GLBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), ZC_GLCullFace(true)) };
    case G_DL_AlphaBlending_ParticleFlame: return { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>
            (0, ZC_GLDepth(true, GL_FALSE), ZC_GLStencil(false), ZC_GLBlend(true, GL_SRC_ALPHA, GL_ONE), ZC_GLCullFace(false)) };
    default: assert(false); return nullptr;
    }
}


    // G_DL_CubeMap = ZC_DL_Drawing - 50,
    // G_DL_AlphaBlending_ParticleStar = G_DL_CubeMap + 1,
    // G_DL_AlphaBlending_ParticlesWind = ZC_DrawerLevels::TextScene + 1,
    // G_DL_AlphaBlending_PlatformDisappear = G_DL_AlphaBlending_PlatformDisappear + 1,