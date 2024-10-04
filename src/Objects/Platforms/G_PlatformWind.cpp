#include "G_PlatformWind.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>
#include <System/G_Func.h>

G_PlatformWind::G_PlatformWind(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0)
{}

void G_PlatformWind::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformWind::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
}

void G_PlatformWind::VDeactivatePlatform_P()
{
    ch_d.wind_phase = wind_end;      //  stop activity
    ch_d.deactivate_color = ZC_UnpackUINTtoFloat_RGB(this->unColor);
    ch_d.wind_time = 0.f;
}

// void G_PlatformWind::VCallback_Collision_P(const ZC_CO_CollisionResult& coll_result)
// {
//     G_Object* pObj = static_cast<G_Object*>(coll_result.pObj->GetHolder());
//     if (pObj->VGetType_O() & G_OT__Pushable && !(ZC_Find(ch_d.pushing_objs, pObj)))
//         ch_d.pushing_objs.emplace_back(pObj);     //  add pushable object in list if it is not yet
//     if (!(ecUpdater.IsConnected())) ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformWind::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
// }

void G_PlatformWind::Callback_Updater(float time)
{
    if (!ch_d.wind_started)
    {
        ch_d.wind_time += time;
        if (ch_d.wind_time >= seconds_to_start_wind)    //  time to die (start wind)
        {
            ch_d.wind_started = true;
            ch_d.wind_time -= seconds_to_start_wind;
            ch_d.wind_power = min_wind_power;
                //  first wind diraction
            ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).Rotate(ZC_Random::GetRandomInt(- ZC_angle_360i, ZC_angle_360i), { 0.f, 0.f, 1.f }) * ZC_Vec4<float>(0.f, 1.f, 0.f, 1.f));
        }
        else    //  activate platform
        {        //  start coloring platform to 0.4 (min value) using 2 secs range (while platform activate)
            float azure_g_b = min_wind_power / max_wind_power;
            this->unColor = G_InterpolateColor(G_Platform::start_lightning_color, { 0.f, azure_g_b, azure_g_b }, ch_d.wind_time / seconds_to_start_wind);
                                                                                            //  ADD VOLUME CONTROLL SOUND
        }
    }
    else
    {
        switch (ch_d.wind_phase)
        {
        case wind_up:
        {
            ch_d.wind_time += time;
            if (ch_d.wind_time >= seconds_on_one_windPhase)
            {
                ch_d.wind_time = ch_d.wind_time - seconds_on_one_windPhase;
                ch_d.wind_phase = wind_max;
                ch_d.wind_power = max_wind_power;
            }
            else ch_d.wind_power += ((max_wind_power - min_wind_power) * (time / seconds_on_one_windPhase));
        } break;
        case wind_max:
        {
            ch_d.wind_time += time;
            if (ch_d.wind_time >= seconds_on_one_windPhase) ch_d.wind_phase = wind_down;
        } break;
        case wind_down:
        {
            ch_d.wind_time -= time;
            if (ch_d.wind_time <= 0.f)
            {
                ch_d.wind_phase = wind_min;
                ch_d.angle_to_new_dir = ZC_Random::GetRandomInt(0, 180);    //  calculate random angle rotate on in wind_minphase
            }
            ch_d.wind_power -= ((max_wind_power - min_wind_power) * (time / seconds_on_one_windPhase));
        } break;
        case wind_min:
        {
            ch_d.wind_time += time;
            if (ch_d.wind_time >= seconds_on_one_windPhase)
            {
                ch_d.wind_time = seconds_on_one_windPhase - ch_d.wind_time;
                ch_d.wind_phase = wind_up;
            }
                //  rotate wind to new direction
            ch_d.wind_dir_cur = ZC_Vec::Vec4_to_Vec3(ZC_Mat4<float>(1.f).
                Rotate(ch_d.angle_to_new_dir * time / seconds_on_one_windPhase, { 0.f, 0.f, 1.f }) * ch_d.wind_dir_cur);
        } break;
        case wind_end:      //  deactivate platform
        {
            ch_d.wind_time += time;
            if (ch_d.wind_time >= seconds_deactivate)
            {
                ch_d = {};    //  activity stoped
                ecUpdater.Disconnect();
                this->unColor = 0;  //  default color
            }
            else this->unColor = G_InterpolateColor(ch_d.deactivate_color, G_Platform::default_lightning, ch_d.wind_time / seconds_deactivate);
            return;
        } break;
        }
        float zero_one_coef = ch_d.wind_power / max_wind_power;
        this->unColor = ZC_PackColorFloatToUInt_RGB(0.f, zero_one_coef, zero_one_coef);
                                                                                            //  ADD VOLUME CONTROLL SOUND
        for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )
        {
            ZC_Vec4<float>& platform_pos = (*(this->upCO->GetModelMatrix()))[3];
            ZC_Vec3<float> obj_pos = (*(iter))->VGetPosition_O();
            float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_pos[0], platform_pos[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
            if (distance < (*(iter))->upCO->GetFigure().radius + this->upCO->GetFigure().radius)
            {
                (*(iter))->VPushObjectInDirection_O(G_PushSet(ch_d.wind_dir_cur, ch_d.wind_power));    //  push object
                ++iter;
            }
            else iter = this->objects_on_platform.erase(iter);   //  object out of cylindric radius of the platform, stop pushing them
        }
    }
}