#pragma once

#define SETUP__G_PARTICLE_SYSTEM

#ifdef SETUP__G_PARTICLE_SYSTEM
#include "G_ParticleSystem.h"
#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
#include <ZC/GUI/ZC_GUI__Switch.h>
#include <ZC/GUI/ZC_GUI__Text.h>
#include <GUI/Text/G_GUI_Fonts.h>
// #include <ZC/GUI/ZC_GUI__CheckBox.h>
#include <ZC/GUI/ZC_GUI__ColorManipulator.h>

struct G_PS_Setup
{
    G_ParticleSystem* pPS = nullptr;

    static inline float button_text_dist = 5.f;

    ZC_GUI__WinImmutable win;
        //  count
    ZC_GUI__ButtonNumberText<ui_zc> bnt__particles_count;
        //  spawn shape
    std::wstring wstr_spawn_shape__radius = L"Radius";
    std::wstring wstr_spawn_shape__half_length = L"Half length";
    ZC_GUI__Text t__spawn_shape;
    ZC_GUI__SwitchDropDown sdd__spawn_shape__shape;
    ZC_GUI__ButtonNumberText<uch_zc> bnt__spawn_shape__fill_to_center;
        //  spawn mat model
    ZC_GUI__Text t__spawn_mat_model;
    ZC_GUI__Text t__spawn_mat_model__tanslate;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_x;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__translate_z;
    ZC_GUI__Text t__spawn_mat_model__rotate;
    ZC_GUI__SwitchDropDown sdd__spawn_mat_model__rotate_order;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_X;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_Y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__rotate_Z;
    ZC_GUI__Text t__spawn_mat_model__scale;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_X;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_Y;
    ZC_GUI__ButtonNumberText<float> bnt__spawn_mat_model__scale_Z;
        //  size
    ZC_GUI__Text t__size;
    ZC_GUI__ButtonNumberText<float> bnt__size__width;
    ZC_GUI__ButtonNumberText<float> bnt__size__height;
        //  life time
    ZC_GUI__Text t__life_time;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_to_start_max;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_min;
    ZC_GUI__ButtonNumberText<float> bnt__life_time__secs_max;
        //  move
    ZC_GUI__Text t__move_set;
    ZC_GUI__Text t__move_set__direction_type;
    ZC_GUI__SwitchDropDown sdd__move_set__direction_type;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_x;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_y;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__move_variable_z;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_power;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_min;
    ZC_GUI__ButtonNumberText<float> bnt__move_set__speed_max;
        //  rotate
    ZC_GUI__Text t__rotate;
    ZC_GUI__Text t__rotate__angle_use;
    ZC_GUI__SwitchDropDown sdd__rotate__angle_use;
    ZC_GUI__ButtonNumberText<float> bnt__rotate__angle_1;
    ZC_GUI__ButtonNumberText<float> bnt__rotate__angle_2;
        //  animation
    ZC_GUI__Text t__animation;
    ZC_GUI__Text t__animation__repeat;
    ZC_GUI__SwitchDropDown sdd__animation__repeat;
    ZC_GUI__ButtonNumberText<float> bnt__animation__tiles_per_second;
    ZC_GUI__Text t__animation__offset_from;
    ZC_GUI__SwitchDropDown sdd__animation__offset_from;
    ZC_GUI__ButtonNumberText<float> bnt__animation__secs_offset_to_start_animation;
        //  color
    ZC_GUI__Text t__color;
    ZC_GUI__ButtonNumberText<float> bnt__color__system_alpha;
    ZC_GUI__Text t__color__rgb_use;
    ZC_GUI__SwitchText st__color__rgb_use;
    ZC_GUI__Text t__color__start;
    ZC_GUI__ButtonNumberText<float> bnt__color__appear_secs;
    ZC_GUI__ColorManipulator cm__color__rgba_start;
    ZC_GUI__Text t__color__end;
    ZC_GUI__ButtonNumberText<float> bnt__color__disappear_secs;
    ZC_GUI__ColorManipulator cm__color__rgba_end;
        //  collision
    ZC_GUI__Text t__collision;
    ZC_GUI__ButtonNumberText<float> bnt__collision__particle_radius;
    ZC_GUI__Text t__collision__set_life_time;
    ZC_GUI__SwitchDropDown sdd__collision__set_life_time;
    ZC_GUI__ButtonNumberText<float> bnt__collision__move_speed;
    
    G_PS_Setup(G_ParticleSystem* _pPS);
    
    void Particles_count(ui_zc v);
        //  spawn shape
    void Spawn_shape__shape(ui_zc v);
    void Spawn_shape__fill_to_center(uch_zc v);
        //  spawn mat model
    void Spawn_Mat_model__position_x(float v);
    void Spawn_Mat_model__position_y(float v);
    void Spawn_Mat_model__position_z(float v);
    void Spawn_Mat_model__rotate_order(ui_zc v);
    void Spawn_Mat_model__rotate_X(float v);
    void Spawn_Mat_model__rotate_Y(float v);
    void Spawn_Mat_model__rotate_Z(float v);
    void Spawn_Mat_model__scale_X(float v);
    void Spawn_Mat_model__scale_Y(float v);
    void Spawn_Mat_model__scale_Z(float v);
        //  size
    void Size__width(float v);
    void Size__height(float v);
        //  life time
    void Life_time__secs_to_start_max(float v);
    void Life_time__secs_min(float v);
    void Life_time__secs_max(float v);
        //  move
    void Move_set__direction_type(ui_zc v);
    void Move_set__move_variable_x(float v);
    void Move_set__move_variable_y(float v);
    void Move_set__move_variable_z(float v);
    void Move_set__speed_power(float v);
    void Move_set__speed_min(float v);
    void Move_set__speed_max(float v);
        //  rotate
    void Rotate__angle_use(ui_zc v);
    void Rotate__angle_1(float v);
    void Rotate__angle_2(float v);
        //  animation
    void Animation__repeat(ui_zc v);
    void Animation__tiles_per_second(float v);
    void Animation__offset_from(ui_zc v);
    void Animation__secs_offset_to_start_animation(float v);
        //  color
    void Color__system_alpha(float v);
    void Color__appear_secs(float v);
    void Color__rgb_use(uint v);
    void Color__rgba_start(float r, float g, float b, float a);
    void Color__disappear_secs(float v);
    void Color__rgba_end(float r, float g, float b, float a);
        //  collision action
    void Collision__particle_radius(float v);
    void Collision__set_life_time(ui_zc v);
    void Collision__move_speed(float v);
};
#endif SETUP__G_PARTICLE_SYSTEM