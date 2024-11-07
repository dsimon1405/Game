#include "G_PlatformDamage.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <GamePlay/G_Map.h>
#include <System/G_Func.h>

G_PlatformDamage::G_PlatformDamage(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, new G_GameSoundSet(GetSounds()), G_PT__Damage)
{
    float dist_to_plat = ZC_Vec::Length(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3]));    //  distance from {0,0,0} to platform center
    const float min_dmg = 5.f;
    const float max_dmg = 15.f;
    damage = std::round(min_dmg + ((max_dmg - min_dmg) * (dist_to_plat / G_Map::map_radius)));
}

void G_PlatformDamage::VAddObjectOnPlatform_P(G_Object* pObj_add)
{
    if (pObj_add->VGetTypeMask_IO() & G_OT__Damageable)
    {
        this->objects_on_platform.emplace_back(pObj_add);
        if (objects_on_platform.size() == 1)
        {
            ch_d.is_active = true;
            ecUpdater.NewConnection(ZC__Updater::Connect({ &G_PlatformDamage::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
            this->upSK->SetSoundState(G_SN__platform_activation, ZC_SS__Play);
            this->upSK->SetSoundState(G_SN__platform_dmg_load_dmg, ZC_SS__PlayLoop);
            this->upSK->SetVolume(G_SN__platform_dmg_load_dmg, 0.f);
        }
    }
}

void G_PlatformDamage::VDeactivatePlatform_P()
{
    ch_d.is_active = false;
    ch_d.deactivate_color = ZC_UnpackUINTtoFloat_RGB(this->unColor);
    ch_d.deactivate_sound_load_dmg_volume = this->upSK->GetVolume(G_SN__platform_dmg_load_dmg);
    ch_d.dmg_time = 0.f;
}

std::vector<G_GameSound> G_PlatformDamage::GetSounds()
{
    std::vector<G_GameSound> sounds;
    sounds.reserve(3);
    sounds.emplace_back(G_GameSound(G_SN__platform_activation));
    sounds.emplace_back(G_GameSound(G_SN__platform_dmg_make_dmg));
    sounds.emplace_back(G_GameSound(G_SN__platform_dmg_load_dmg));
    return sounds;
}

void G_PlatformDamage::Callback_Updater(float time)
{
    static const float seconds_to_dmg = 2.f;
    static const float half_seconds_between_dmg = seconds_to_dmg / 2.f;
    static const ZC_Vec3<float> dmg_color { 1.f, 0.f, 0.f };

    if (ch_d.is_active)
    {
        ch_d.dmg_time += time;
            //  changing color, must be before changing time ch_d.dmg_time second time
        if (ch_d.is_first_dmg)  //  on first dmg need to change dmg color from color_white to dmg_color full dmg_time, in other case: first half to deactivate, second half time to activate
        {
            float time_coef = ch_d.dmg_time > seconds_to_dmg ? 1.f : ch_d.dmg_time / seconds_to_dmg;
            this->unColor = G_InterpolateColor_PackToUInt(G_Platform::color_white, dmg_color, time_coef);
            this->upSK->SetVolume(G_SN__platform_dmg_load_dmg, time_coef);
            if (ch_d.dmg_time >= seconds_to_dmg) ch_d.is_first_dmg = false;     //  first dmg reached
        }
        else if (ch_d.dmg_time >= half_seconds_between_dmg)    //  activate
        {
            float time_coef = ch_d.dmg_time > seconds_to_dmg ? 1.f : (ch_d.dmg_time - half_seconds_between_dmg) / half_seconds_between_dmg;
            this->unColor = G_InterpolateColor_PackToUInt(G_Platform::color_default, dmg_color, time_coef);
            this->upSK->SetVolume(G_SN__platform_dmg_load_dmg, time_coef);
        }
        else   //  deactivate
        {
            this->unColor = G_InterpolateColor_PackToUInt(dmg_color, G_Platform::color_default, ch_d.dmg_time);
            this->upSK->SetVolume(G_SN__platform_dmg_load_dmg, 1.f - ch_d.dmg_time);
        }

        if (ch_d.dmg_time >= seconds_to_dmg)
        {
            ch_d.dmg_time -= seconds_to_dmg;
            for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )   //  check is object still in platforms cylindric radius, if yes damage, otherwise erase from list
            {
                if (this->IsObjectInPlatformRadiusXY(*iter))
                {
                    (*(iter))->VDamageObject_IO(damage, G_OT__Platform);
                    ++iter;
                }
                else iter = this->EraseObjectFromPlatform(iter);   //  object out of cylindric radius of the platform, erase them
            }
            if (this->objects_on_platform.size() == 0) VDeactivatePlatform_P();
            this->upSK->SetSoundState(G_SN__platform_dmg_make_dmg, ZC_SS__Play);
        }
    }
    else    //  deactivate
    {
        static const float seconds_deactivate = 1.f;  
     
        ch_d.dmg_time += time;
        if (ch_d.dmg_time >= seconds_deactivate)   //  deactivated
        {
            ecUpdater.Disconnect();
            ch_d = {};
            this->unColor = 0;
            this->upSK->SetSoundState(G_SN__platform_dmg_load_dmg, ZC_SS__Stop);
        }
        else
        {
            this->unColor = G_InterpolateColor_PackToUInt(ch_d.deactivate_color, G_Platform::color_default, ch_d.dmg_time);
            this->upSK->SetVolume(G_SN__platform_dmg_load_dmg, 1.f - ch_d.dmg_time);
        }
    }
}