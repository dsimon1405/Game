#pragma once

#include <System/G_LightSet.h>
#include <ZC/Events/ZC_EC.h>

class G_Platform;

class G_PlatformLightHolder
{
public:
    void SetLightHolder(G_Platform* _pPlatform);
    G_Platform* GetLightHolder();
        //  deactivate light if _pPlatform is active light
    void DeactivatePlatformLight(G_Platform* _pPlatform);

private:
    G_Platform* pPlatform = nullptr;

    G_LightSet last_light_set;

    ZC_EC ecUpdater;

    void Callback_Updater(float time);
};