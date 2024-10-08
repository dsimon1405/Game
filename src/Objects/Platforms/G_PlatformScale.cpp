#include "G_PlatformScale.h"

#include <Model/G_Models.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>
#include <ZC/Video/ZC_SWindow.h>
#include <System/G_UpdaterLevels.h>
#include <System/G_Func.h>

G_PlatformScale::G_PlatformScale(const G_PlatformTransforms& _plat_trans)
    : G_Platform(_plat_trans, G_MN__Platform_cylinder_black, 0, nullptr)
{}

void G_PlatformScale::VAddObjectOnPlatform(G_Object* pObj_add)
{
    this->objects_on_platform.emplace_back(pObj_add);
    if (objects_on_platform.size() == 1)
    {
        ch_d.scale_state = SS_start;
        ecUpdater.NewConnection(ZC_SWindow::ConnectToUpdater({ &G_PlatformScale::Callback_Updater, this }, G_UL__game_play));   //  connect to update if it is not yet
    }
}

void G_PlatformScale::Callback_Updater(float time)
{
    ch_d.scale_time += time;
    switch (ch_d.scale_state)
    {
    case SS_start: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_down;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
        }
        else this->unColor = G_InterpolateColor(G_Platform::color_white, scale_color, ch_d.scale_time / seconds_pause);
    } break;
    case SS_scale_down: 
    {
        if (ch_d.scale_time >= seconds_scale)
        {
            ch_d.scale_state = SS_pause_on_down;
            ch_d.scale_time -= seconds_scale;
                //  update model matrix and radius for collision object
            ZC_Vec3<float> scale(scale_min, scale_min, scale_Z);
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
            // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));

            CheckPlatformHabitability(scale_min);
        }
        else
        {
            float scale_coef = scale_max - ((scale_max - scale_min) * (ch_d.scale_time / seconds_scale));
            ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                //  update model matrix and radius for collision object
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
            // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));
        }
    } break;
    case SS_pause_on_down: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_up;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
        }
        else 
        {
            float half_pause_time = seconds_pause / 2.f;
            if (ch_d.scale_time < half_pause_time) this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, ch_d.scale_time / half_pause_time);
            else this->unColor = G_InterpolateColor(G_Platform::color_default, scale_color, (ch_d.scale_time - half_pause_time) / half_pause_time);
        }
    } break;
    case SS_scale_up: 
    {
        if (ch_d.scale_time >= seconds_scale)
        {
            ch_d.scale_state = SS_pause_on_up;
            ch_d.scale_time -= seconds_scale;
                //  update model matrix and radius for collision object
            ZC_Vec3<float> scale(scale_max, scale_max, scale_Z);
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
            // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));
            
            CheckPlatformHabitability(scale_max);
        }
        else
        {
            float scale_coef = scale_min + ((scale_max - scale_min) * (ch_d.scale_time / seconds_scale));
            ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                //  update model matrix and radius for collision object
            this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
            // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
            // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));
        }
    } break;
    case SS_pause_on_up: 
    {
        if (ch_d.scale_time >= seconds_pause)
        {
            ch_d.scale_state = SS_scale_down;
            ch_d.scale_time -= seconds_pause;
            this->unColor = scale_color_packed;
        }
        else 
        {
            float half_pause_time = seconds_pause / 2.f;
            if (ch_d.scale_time < half_pause_time) this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, ch_d.scale_time / half_pause_time);
            else this->unColor = G_InterpolateColor(G_Platform::color_default, scale_color, (ch_d.scale_time - half_pause_time) / half_pause_time);
        }
    } break;
    case SS_end: 
    {
        if (ch_d.scale_time >= deactivate_time)
        {
            ecUpdater.Disconnect();
            ch_d = {};
            this->unColor = 0;
            if (ch_d.deactivate_scale != scale_max)     //  set default scaling
            {
                ZC_Vec3<float> scale(scale_max, scale_max, scale_Z);
                this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
                // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
                // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));
            }
        }
        else
        {
            float time_coef = ch_d.scale_time / deactivate_time;
            if (ch_d.deactivate_scale != scale_max)     //  platfrom start deactivate not on max scale, need scale to max
            {
                float scale_coef = scale_min + ((scale_max - scale_min) * time_coef);
                ZC_Vec3<float> scale(scale_coef, scale_coef, scale_Z);
                    //  update model matrix and radius for collision object
                this->platf_trans.Update_scale(scale, G_MN__Platform_cylinder_black);
                // this->upCO->UpdateModelMatrix(ZC_Mat4<float>(1.f).Translate(ZC_Vec::Vec4_to_Vec3((*(this->upCO->GetModelMatrix()))[3])).Scale(scale));
                // this->upCO->UpdateRadius(G_Models::CalculateRadius(G_MN__Platform_cylinder_black, scale));
            }
            this->unColor = G_InterpolateColor(scale_color, G_Platform::color_default, time_coef);
        }
    } break;
    }
}

void G_PlatformScale::CheckPlatformHabitability(float scale_val)
{
    for (auto iter = this->objects_on_platform.begin(); iter != this->objects_on_platform.end(); )   //  check is object still in platforms cylindric radius
    {
        iter = this->IsObjectInPlatformRadiusXY(*iter) ? ++iter : this->objects_on_platform.erase(iter);   //  insecond case object out of cylindric radius of the platform, stop pushing them
    }
    if (this->objects_on_platform.size() == 0)      //  stop activity
    {
        ch_d.scale_state = SS_end;
        ch_d.scale_time = 0.f;
        ch_d.deactivate_scale = scale_val;
    }
}


            // for (auto iter = ch_d.objs.begin(); iter != ch_d.objs.end(); )   //  check is object still in platforms cylindric radius
            // {
            //     ZC_Vec4<float>& platform_pos = (*(this->upCO->GetModelMatrix()))[3];
            //     ZC_Vec3<float> obj_pos = (*(iter))->VGetPosition_O();
            //     float distance = ZC_Vec::Length(ZC_Vec3<float>(platform_pos[0], platform_pos[1], 0.f) - ZC_Vec3<float>(obj_pos[0], obj_pos[1], 0.f));
            //     iter = distance < (*(iter))->upCO->GetFigure().radius + this->upCO->GetFigure().radius ? ++iter : ch_d.objs.erase(iter);   //  insecond case object out of cylindric radius of the platform, stop pushing them
            // }
            // if (ch_d.objs.size() == 0)      //  stop activity
            // {
            //     ch_d.scale_state = SS_end;
            //     ch_d.scale_time = 0.f;
            //     ch_d.deactivate_scale = scale_max;
            // }