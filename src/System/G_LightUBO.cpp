#include "G_LightUBO.h"

#include <ZC/Video/OpenGL/Buffer/ZC_UBOs.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>

void G_LightUBO::Init()
{
    upUBO_light = new ZC_UBO(ZC_UBO_BP__Light);
    
    upUBO_light->GLNamedBufferStorage(sizeof(light_sets), light_sets, GL_DYNAMIC_STORAGE_BIT);    //  sizeof(light_sets) - array give byte size
    // upTest_light = new TestLight();
    // upUBO_light->GLNamedBufferStorage(sizeof(light_sets) + sizeof(upTest_light->test_params), light_sets, GL_DYNAMIC_STORAGE_BIT);    //  sizeof(light_sets) - array give byte size
    
    ZC_UBOs::AddUpdateFunctionToUBOs(upUBO_light.Get(), { &G_LightUBO::Callback_UBOLightUpdate }, ZC_AddToRenderer);

    lights_to_update.reserve(G_LN__sizeof);
}

void G_LightUBO::Destroy()
{
    upUBO_light = nullptr;
}

void G_LightUBO::UpdateLightData(G_LightName light_name, const G_LightSet& light_set)
{
    assert(light_name != G_LN__sizeof);     //  not a light name
    light_sets[light_name] = light_set;
    if (!ZC_Find(lights_to_update, light_name)) lights_to_update.emplace_back(light_name);
}

void G_LightUBO::Callback_UBOLightUpdate()
{
    switch (int(lights_to_update.size()))
    {
    case 0: break;
    case 1: upUBO_light->GLNamedBufferSubData(0, sizeof(G_LightSet), &(light_sets[lights_to_update[0]])); break;   //  only one light to update
    case G_LN__sizeof: upUBO_light->GLNamedBufferSubData(0, sizeof(light_sets), light_sets); break;   //  update all ubo
    default: assert(false); break;
    }
    lights_to_update.clear();


    // upUBO_light->GLNamedBufferSubData(sizeof(light_sets), sizeof(upTest_light->test_params), &(upTest_light->test_params));
}