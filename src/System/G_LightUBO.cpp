#include "G_LightUBO.h"

#include <ZC/Video/OpenGL/Buffer/ZC_UBOs.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>

void G_LightUBO::Init()
{
    upUBO_light = new ZC_UBO(ZC_UBO_BP__Light);
    
    upUBO_light->GLNamedBufferStorage(sizeof(light), &light, GL_DYNAMIC_STORAGE_BIT);    //  sizeof(light_sets) - array give byte size
    
    ZC_UBOs::AddUpdateFunctionToUBOs(upUBO_light.Get(), { &G_LightUBO::Callback_UBOLightUpdate }, ZC_AddToRenderer);
}

void G_LightUBO::Destroy()
{
    upUBO_light = nullptr;
}

void G_LightUBO::UpdateLightSet(G_LightName light_name, const G_LightSet& light_set)
{
    assert(light_name != G_LN__sizeof);     //  not a light name
    light.light_sets[light_name] = light_set;
    update_light = true;
}

void G_LightUBO::UpdateAttenuation(G_LightAttenuationName att_name, const G_LightAttenuation& attenuation)
{
    assert(att_name != G_LAN_sizeof);     //  not a light name
    light.attenuations[att_name] = attenuation;
    update_attenuation = true;
}

void G_LightUBO::Callback_UBOLightUpdate()
{
    if (update_light && update_attenuation) upUBO_light->GLNamedBufferSubData(0, sizeof(light), &light);
    else if (update_light) upUBO_light->GLNamedBufferSubData(offsetof(Light, light_sets), sizeof(light.light_sets), light.light_sets);
    else if (update_attenuation) upUBO_light->GLNamedBufferSubData(offsetof(Light, attenuations), sizeof(light.attenuations), light.attenuations);
    update_light = false;
    update_attenuation = false;
}