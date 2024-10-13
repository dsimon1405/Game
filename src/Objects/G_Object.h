#pragma once

#include "G_IObject.h"
#include <ZC/Video/OpenGL/Renderer/ZC_DSController.h>
#include <ZC/Collision/ZC_CollisionObject.h>
#include <Model/G_ModelSet.h>
#include <Sound/G_GameSoundSet.h>

struct G_Object : public G_IObject
{
    ZC_DSController dsCon;
    ZC_uptr<ZC_CollisionObject> upCO;
    ZC_uptr<G_GameSoundSet> upSK;
    uint unColor = 0;   //  uniform of adding color in sphere.fs. Pack shema bytes 2x10x10x10
    float unAlpha = 1.f;

    G_Object(G_ModelName modelName, int texSetId, ZC_uptr<ZC_CollisionObject>&& _upCO, ZC_uptr<G_GameSoundSet>&& _upSK);
    virtual ~G_Object() = default;

    void SetDefaultState();
    void SetSoundSetToDefault();
    float GetRadius();

    ZC_Vec3<float> VGetPosition_IO() noexcept override;
    float VGetRadius_IO() const noexcept override;

private:
        //  make all object params default
    virtual void VSetDefaultState_O() {}
};