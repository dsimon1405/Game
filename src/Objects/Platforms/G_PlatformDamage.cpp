#include "G_PlatformDamage.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <GamePlay/G_Map.h>
#include <System/G_Func.h>

G_PlatformDamage::G_PlatformDamage(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0)
{
    float dist_to_plat = ZC_Vec::Length(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3]));    //  distance from {0,0,0} to platform center
    const float min_dmg = 5.f;
    const float max_dmg = 15.f;
    damage = std::round(min_dmg + ((max_dmg - min_dmg) * (dist_to_plat / G_Map::map_radius)));
}

void G_PlatformDamage::VAddObjectOnPlatform(G_Object* pObj_add)
{
    if (pObj_add->VGetType_O() & G_OT__Damageable)
    {
        this->objects_on_platform.emplace_back(pObj_add);
        if (objects_on_platform.size() == 1)
        {
            ch_d.is_active = true;
            ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformDamage::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
        }
    }
}

void G_PlatformDamage::VDeactivatePlatform_P()
{
    ch_d.is_active = false;
    ch_d.deactivate_color = ZC_UnpackUINTtoFloat_RGB(this->unColor);
    ch_d.dmg_time = 0.f;
}

void G_PlatformDamage::Callback_Updater(float time)
{
    if (ch_d.is_active)
    {
        ch_d.dmg_time += time;
            //  changing color, must be before changing time ch_d.dmg_time second time
        float half_sec_to_dmg = seconds_to_dmg / 2.f;
        if (ch_d.is_first_dmg)  //  on first dmg nned to change dmg color from color_white to dmg_color full dmg_time, in other case: first half to deactivate, second half time to activate
        {
            this->unColor = G_InterpolateColor(G_Platform::color_white, dmg_color, ch_d.dmg_time > seconds_to_dmg ? 1.f : ch_d.dmg_time / seconds_to_dmg);
            if (ch_d.dmg_time >= seconds_to_dmg) ch_d.is_first_dmg = false;     //  first dmg reached
        }
        else if (ch_d.dmg_time >= half_sec_to_dmg)
            this->unColor = G_InterpolateColor(G_Platform::color_default, dmg_color,
                ch_d.dmg_time > seconds_to_dmg ? 1.f : (ch_d.dmg_time - half_sec_to_dmg) / half_sec_to_dmg);    //  activate
        else this->unColor = G_InterpolateColor(dmg_color, G_Platform::color_default, ch_d.dmg_time);   //  deactivate
        if (ch_d.dmg_time >= seconds_to_dmg)
        {
            ch_d.dmg_time -= seconds_to_dmg;
            for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )   //  check is object still in platforms cylindric radius, if yes damage, otherwise erase from list
            {
                if (this->IsObjectInPlatformRadiusXY(*iter))
                {
                    (*(iter))->VDamagerObject_O(damage);
                    ++iter;
                }
                else iter = this->objects_on_platform.erase(iter);   //  object out of cylindric radius of the platform, stop pushing them
            }
            if (this->objects_on_platform.size() == 0) VDeactivatePlatform_P();
                                                                                                    //  DAMAGE SOUND
        }
    }
    else    //  deactivate
    {
        ch_d.dmg_time += time;
        if (ch_d.dmg_time >= 1.f)   //  deactivated
        {
            ecUpdater.Disconnect();
            ch_d = {};
            this->unColor = 0;
        }
        else
        {
            this->unColor = G_InterpolateColor(ch_d.deactivate_color, G_Platform::color_default, ch_d.dmg_time);
                //          LOW VOLUME
        }
    }
}