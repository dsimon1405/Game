#include "G_PlatformLightHolder.h"

#include "G_Platform.h"
#include <System/G_LightUBO.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/ZC__System.h>

void G_PlatformLightHolder::SetLightHolder(G_Platform* _pPlatform)
{
    // assert(pPlatform != _pPlatform);    //  why re-add
    assert(_pPlatform);

    pPlatform = _pPlatform;
        //  start platform have not light
    if (pPlatform->platform_type == G_PT__Start)
    {
        ecUpdater.Disconnect();
        G_LightUBO::UpdateLightData(G_LN__Platform, G_LightSet{ .pos = { 0.f, 0.f, 0.f }, .color = 0u });   //  set default data
    }
    else
    {
        if (!(ecUpdater.IsConnected()))   //  connect to G_UL__game_particles, to be after all platform update at G_UL__game_play level
            ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformLightHolder::Callback_Updater, this }, G_UL__game_particles));
    }
}

G_Platform* G_PlatformLightHolder::GetLightHolder()
{
    return pPlatform;
}

void G_PlatformLightHolder::DeactivatePlatformLight(G_Platform* _pPlatform)
{
    if (pPlatform != _pPlatform) return;
    pPlatform = nullptr;
    ecUpdater.Disconnect();
}

void G_PlatformLightHolder::Callback_Updater(float time)
{
    if (pPlatform->platform_type == G_PT__Start) return;    //  no light

    G_LightUBO::UpdateLightData(G_LN__Platform, pPlatform->GetLightSet_P());
}