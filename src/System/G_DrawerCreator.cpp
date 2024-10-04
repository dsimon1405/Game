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
    case G_DL_AlphaBlending: return                               //  wrong way! (depth must be write in texture and then make depth test from texture for fragment)
        { new ZC_DrawerFL<const ZC_ShProg*, ZC_TexturesHolder, const ZC_VAO*, ZC_RLDData_Uniforms_GLDraw>(0, true, true, false, ZC_GLBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)) };
    default: assert(false); return nullptr;
    }
}