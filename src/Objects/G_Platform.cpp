#include "G_Platform.h"

#include <Model/G_Models.h>

G_Platform::G_Platform(const ZC_Mat4<float>& model, const ZC_Vec3<float>& scale)
    : G_Object(G_ModelName::G_MN__Platform_cylinder_black, 0, new ZC_CollisionObject(G_Models::GetModel_COFigure(G_MN__Platform_cylinder_black, scale), ZC_C0_Type::ZC_COT__StaticSoloCollision, this, nullptr, model))
{}

G_Object_Type G_Platform::GetType() const
{
    return G_O__PlatformQuad;
}
