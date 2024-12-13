#include "G_PS_Setup.h"

#ifdef SETUP__G_PARTICLE_SYSTEM
#include <System/G_RenderLevel.h>
#include <ZC/Tools/ZC_Random.h>
#include <ZC/GUI/ZC__GUI.h>

G_PS_Setup::G_PS_Setup(G_ParticleSystem* _pPS)
    : pPS(_pPS),
    win(ZC_WOIData(350.f, 600.f, -300.f, 0.f, ZC_WOIF__X_Center_Pixel | ZC_WOIF__Y_Center_Pixel), ZC_GUI_WF__Movable | ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
    bnt__particles_count(ZC_GUI_ButtonNumber<ui_zc>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.particles_count, 1, 10000, 1, 2, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Particles_count, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Particles amount", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  spawn shape
    t__spawn_shape(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Spawn Shape ", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__spawn_shape__shape(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Circle", L"Sphere", L"Square", L"Cube", L"Hemisphere" }, ui_zc(pPS->c_ps_src.spawn_shape.shape), 0.f, 0.f, { &G_PS_Setup::Spawn_shape__shape, this }),
    bnt__spawn_shape__fill_to_center(ZC_GUI_ButtonNumber<uch_zc>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0, 0, pPS->c_ps_src.spawn_shape.fill_to_center * 100, 0, 100, 1, 5, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_shape__fill_to_center, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Fill to center %", true, G_GUI_Fonts::Get(G_GUI_FN__Arial_20)->CalculateWstrWidth(wstr_spawn_shape__half_length), ZC_GUI_TextAlignment::Right, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  spawn mat model
    t__spawn_mat_model(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Spawn Mat Model", true, 0, ZC_GUI_TextAlignment::Left),
    t__spawn_mat_model__tanslate(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Translate", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__spawn_mat_model__translate_x(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[0], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__position_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__translate_y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[1], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__position_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__translate_z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.translate[2], -10000.f, 10000.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__position_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    t__spawn_mat_model__rotate(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Rotate", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__spawn_mat_model__rotate_order(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"x->y->z", L"x->z->y", L"y->x->z", L"z->x->y", L"y->z->x", L"z->y->x" }, pPS->c_ps_src.spawn_mat_model.rotate_order, 0.f, 0.f, { &G_PS_Setup::Spawn_Mat_model__rotate_order, this }),
    bnt__spawn_mat_model__rotate_X(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_X, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__rotate_X, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__rotate_Y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__rotate_Y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__rotate_Z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z, -360.f, 360.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__rotate_Z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    t__spawn_mat_model__scale(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Scale", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__spawn_mat_model__scale_X(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[0], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__scale_X, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__scale_Y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[1], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__scale_Y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__spawn_mat_model__scale_Z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.spawn_mat_model.scale[2], 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Spawn_Mat_model__scale_Z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  size
    t__size(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Size", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__size__width(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.size.width, 0.01f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Size__width, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Width", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__size__height(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.size.height, 0.01f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Size__height, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Height", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  life time
    t__life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Life Time (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__life_time__secs_to_start_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_to_start_max, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Life_time__secs_to_start_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Life start at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__life_time__secs_min(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_min, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Life_time__secs_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Duration min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__life_time__secs_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.life_time.secs_max, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Life_time__secs_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Duration max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  move set
    t__move_set(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Move (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
    t__move_set__direction_type(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Type (local space)", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__move_set__direction_type(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"From center", L"Direction", L"Point" }, pPS->c_ps_src.move.direction_type, 0.f, 0.f, { &G_PS_Setup::Move_set__direction_type, this }),
    bnt__move_set__move_variable_x(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[0], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__move_variable_x, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"X", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__move_set__move_variable_y(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[1], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__move_variable_y, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__move_set__move_variable_z(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.variable[2], -10000.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__move_variable_z, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__move_set__speed_power(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_power, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__speed_power, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed power", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__move_set__speed_min(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_min, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__speed_min, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__move_set__speed_max(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.move.speed_max, 0.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Move_set__speed_max, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Speed max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  rotate
    t__rotate(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Rotate (degrees)", true, 0, ZC_GUI_TextAlignment::Left),
    t__rotate__angle_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle use", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__rotate__angle_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Constant", L"Random constant", L"Random value" }, pPS->c_ps_src.rotate.angle_use, 0.f, 0.f, { &G_PS_Setup::Rotate__angle_use, this }),
    bnt__rotate__angle_1(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.rotate.angle_1, -1000.f, 1000.f, 1.f, 3.f, 1, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Rotate__angle_1, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle 1", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    bnt__rotate__angle_2(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.rotate.angle_2, -1000.f, 1000.f, 1.f, 3.f, 1, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Rotate__angle_2, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Angle 2", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  animation
    t__animation(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Animation (seconds)", true, 0, ZC_GUI_TextAlignment::Left),
    t__animation__repeat(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Repeat", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__animation__repeat(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Loop", L"Single pass" }, pPS->c_ps_src.animation.repaet, 0.f, 0.f, { &G_PS_Setup::Animation__repeat, this }),
    bnt__animation__tiles_per_second(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.animation.tiles_per_second, 0.f, 1000.f, 0.1f, 0.5f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Animation__tiles_per_second, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Tiles per second", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    t__animation__offset_from(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Offset from life", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__animation__offset_from(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Start", L"End" }, pPS->c_ps_src.animation.offset_from, 0.f, 0.f, { &G_PS_Setup::Animation__offset_from, this }),
    bnt__animation__secs_offset_to_start_animation(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.animation.offset_to_start_animation_secs, 0.f, 1000.f, 0.1f, 0.5f, 0, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Animation__secs_offset_to_start_animation, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Start animation at", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
        //  color
    t__color(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Color", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__color__system_alpha(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.color.system_alpha, 0.f, 1.f, 0.1f, 0.2f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Color__system_alpha, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"System alpha", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    t__color__rgb_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Use RGB: ", true, 0, ZC_GUI_TextAlignment::Left),
    st__color__rgb_use(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"Add", L"Replace" }, 0.f, 0.f, true, 10.f, { &G_PS_Setup::Color__rgb_use, this }, pPS->c_ps_src.color.rgb_use),
    t__color__start(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Start", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__color__appear_secs(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.color.appear_secs, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Color__appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Appear (seconds)", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    cm__color__rgba_start(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { &G_PS_Setup::Color__rgba_start, this }, false),
    t__color__end(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"End", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__color__disappear_secs(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_ps_src.color.disappear_secs, 0.f, 100.f, 0.1f, 0.5f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Color__disappear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Disappear (seconds)", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    cm__color__rgba_end(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { &G_PS_Setup::Color__rgba_end, this }, false),
        //  collision
    t__collision(G_GUI_Fonts::Get(G_GUI_FN__Arial_40), L"Collision", true, 0, ZC_GUI_TextAlignment::Left),
    bnt__collision__particle_radius(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_collision.particle_radius, 0.f, 100.f, 0.1f, 0.4f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Collision__particle_radius, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Particle radius", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0))),
    t__collision__set_life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"Jump to life time:", true, 0, ZC_GUI_TextAlignment::Left),
    sdd__collision__set_life_time(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), { L"None", L"Start Animation" }, pPS->c_collision.set_life_time, 0.f, 0.f, { &G_PS_Setup::Collision__set_life_time, this }),
    bnt__collision__move_speed(ZC_GUI_ButtonNumber<float>(G_GUI_Fonts::Get(G_GUI_FN__Arial_20), 0.f, 0.f, pPS->c_collision.move_speed, -1.f, 1.f, 1.f, 5.f, 2, ZC_GUI_TextAlignment::Center, { &G_PS_Setup::Collision__move_speed, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(button_text_dist, ZC_GUI_TFB_Indent_Location::OutOfButtonRight), G_GUI_Fonts::Get(G_GUI_FN__Arial_20), L"% Move speed", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_Pack_UChar_To_UInt_2x10x10x10(230, 230, 230), 0)))
{
    float section_y = 20;
    float sub_section_y = 15;
    float row_y = 10;
    float distacne_x = 10.f;
    float indent_left = 10.f;
        //  particles count
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__particles_count.GetObj() }));
        //  spawn shape
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_shape.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { sdd__spawn_shape__shape.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_shape__fill_to_center.GetObj() }));
        //  spwn mat model
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__tanslate.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_x.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_y.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__translate_z.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__rotate.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { sdd__spawn_mat_model__rotate_order.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_X.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_Y.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__rotate_Z.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__spawn_mat_model__scale.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_X.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_Y.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__spawn_mat_model__scale_Z.GetObj() }));
        //  size
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__size.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__size__width.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__size__height.GetObj() }));
        //  life time
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__life_time.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_to_start_max.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_min.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__life_time__secs_max.GetObj() }));
        //  move
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__move_set.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_power.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_min.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__speed_max.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, sub_section_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__move_set__direction_type.GetObj(), sdd__move_set__direction_type.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_x.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_y.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__move_set__move_variable_z.GetObj() }));
    if (pPS->c_ps_src.move.direction_type == G_PS_Source::Move::DT__from_particles_center)
    {
        bnt__move_set__move_variable_x.SetDrawState(false);
        bnt__move_set__move_variable_y.SetDrawState(false);
        bnt__move_set__move_variable_z.SetDrawState(false);
    }
        //  rotate
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__rotate.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__rotate__angle_use.GetObj(), sdd__rotate__angle_use.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__rotate__angle_1.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__rotate__angle_2.GetObj() }));
    if (pPS->c_ps_src.rotate.angle_use == G_PS_Source::Rotate::AU__contant) bnt__rotate__angle_2.SetDrawState(false);
        //  animation
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__animation.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Bottom), { t__animation__repeat.GetObj(), sdd__animation__repeat.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__animation__tiles_per_second.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, distacne_x, ZC_GUI_RowParams::Y_Center), { t__animation__offset_from.GetObj(), sdd__animation__offset_from.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__animation__secs_offset_to_start_animation.GetObj() }));
        //  color 
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__color__system_alpha.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__rgb_use.GetObj(), st__color__rgb_use.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__start.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__color__appear_secs.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { cm__color__rgba_start.GetObj() }));
    cm__color__rgba_start.SetColor<float>(&(pPS->c_ps_src.color.rgba_start[0]), &(pPS->c_ps_src.color.rgba_start[1]), &(pPS->c_ps_src.color.rgba_start[2]), false);
    cm__color__rgba_start.SetAlpha(pPS->c_ps_src.color.rgba_start[3], false);
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, sub_section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__color__end.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__color__disappear_secs.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { cm__color__rgba_end.GetObj() }));
    cm__color__rgba_end.SetColor<float>(&(pPS->c_ps_src.color.rgba_end[0]), &(pPS->c_ps_src.color.rgba_end[1]), &(pPS->c_ps_src.color.rgba_end[2]), false);
    cm__color__rgba_end.SetAlpha(pPS->c_ps_src.color.rgba_end[3], false);
        //  collision
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::X_Center, section_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__collision.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__collision__particle_radius.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { t__collision__set_life_time.GetObj(), sdd__collision__set_life_time.GetObj() }));
    win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(indent_left, ZC_GUI_RowParams::X_Left, row_y, 0.f, ZC_GUI_RowParams::Y_Center), { bnt__collision__move_speed.GetObj() }));
}

void G_PS_Setup::Particles_count(ui_zc v) { pPS->Set_Particles_count(v); }
void G_PS_Setup::Spawn_shape__shape(ui_zc v) { pPS->Set_SpawnShape__shape(G_PS_Source::SpawnShape::Shape(v)); }
void G_PS_Setup::Spawn_shape__fill_to_center(uch_zc v) { pPS->Set_SpawnShape__fill_to_center(v / 100.f); }
void G_PS_Setup::Spawn_Mat_model__position_x(float v) { pPS->Set_SpawnMatModel__translation({ v, pPS->c_ps_src.spawn_mat_model.translate[1], pPS->c_ps_src.spawn_mat_model.translate[2] }); }
void G_PS_Setup::Spawn_Mat_model__position_y(float v) { pPS->Set_SpawnMatModel__translation({ pPS->c_ps_src.spawn_mat_model.translate[0], v, pPS->c_ps_src.spawn_mat_model.translate[2] }); }
void G_PS_Setup::Spawn_Mat_model__position_z(float v) { pPS->Set_SpawnMatModel__translation({ pPS->c_ps_src.spawn_mat_model.translate[0], pPS->c_ps_src.spawn_mat_model.translate[1], v }); }
void G_PS_Setup::Spawn_Mat_model__rotate_order(ui_zc v) { pPS->Set_SpawnMatModel__rotate(G_PS_Source::SpawnMatModel::RotateOrder(v),
    pPS->c_ps_src.spawn_mat_model.rotate_angle_X, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
void G_PS_Setup::Spawn_Mat_model__rotate_X(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
    v, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
void G_PS_Setup::Spawn_Mat_model__rotate_Y(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
    pPS->c_ps_src.spawn_mat_model.rotate_angle_X, v, pPS->c_ps_src.spawn_mat_model.rotate_angle_Z); }
void G_PS_Setup::Spawn_Mat_model__rotate_Z(float v) { pPS->Set_SpawnMatModel__rotate(pPS->c_ps_src.spawn_mat_model.rotate_order,
    pPS->c_ps_src.spawn_mat_model.rotate_angle_X, pPS->c_ps_src.spawn_mat_model.rotate_angle_Y, v); }
void G_PS_Setup::Spawn_Mat_model__scale_X(float v) { pPS->Set_SpawnMatModel__scale({v, pPS->c_ps_src.spawn_mat_model.scale[1], pPS->c_ps_src.spawn_mat_model.scale[2] }); }
void G_PS_Setup::Spawn_Mat_model__scale_Y(float v) { pPS->Set_SpawnMatModel__scale({pPS->c_ps_src.spawn_mat_model.scale[0], v, pPS->c_ps_src.spawn_mat_model.scale[2] }); }
void G_PS_Setup::Spawn_Mat_model__scale_Z(float v) { pPS->Set_SpawnMatModel__scale({pPS->c_ps_src.spawn_mat_model.scale[0], pPS->c_ps_src.spawn_mat_model.scale[1], v }); }
void G_PS_Setup::Size__width(float v) { pPS->Set_Size__width(v); }
void G_PS_Setup::Size__height(float v) { pPS->Set_Size__height(v); }
void G_PS_Setup::Life_time__secs_to_start_max(float v) { pPS->Set_Life_time__secs_to_start_max(v); }
void G_PS_Setup::Life_time__secs_min(float v) { pPS->Set_Life_time__min(v); }
void G_PS_Setup::Life_time__secs_max(float v) { pPS->Set_Life_time__max(v); }
void G_PS_Setup::Move_set__direction_type(ui_zc v)
{
    bnt__move_set__move_variable_x.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
    bnt__move_set__move_variable_y.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
    bnt__move_set__move_variable_z.SetDrawState(G_PS_Source::Move::DirectionType(v) != G_PS_Source::Move::DT__from_particles_center);
    pPS->Set_Move__direction_type(G_PS_Source::Move::DirectionType(v));
}
void G_PS_Setup::Move_set__move_variable_x(float v) { pPS->Set_Move__move_variable({ v, pPS->c_ps_src.move.variable[1], pPS->c_ps_src.move.variable[2] }); }
void G_PS_Setup::Move_set__move_variable_y(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.variable[0], v, pPS->c_ps_src.move.variable[2] }); }
void G_PS_Setup::Move_set__move_variable_z(float v) { pPS->Set_Move__move_variable({ pPS->c_ps_src.move.variable[0], pPS->c_ps_src.move.variable[1], v }); }
void G_PS_Setup::Move_set__speed_power(float v) { pPS->Set_Move__speed_power(v); }
void G_PS_Setup::Move_set__speed_min(float v) { pPS->Set_Move__speed_min(v); }
void G_PS_Setup::Move_set__speed_max(float v) { pPS->Set_Move__speed_max(v); }
void G_PS_Setup::Rotate__angle_use(ui_zc v)
{
    bnt__rotate__angle_2.SetDrawState(v != G_PS_Source::Rotate::AU__contant);
    pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = G_PS_Source::Rotate::AngleUse(v), .angle_1 = pPS->c_ps_src.rotate.angle_1, .angle_2 = pPS->c_ps_src.rotate.angle_2 });
}
void G_PS_Setup::Rotate__angle_1(float v) { pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = pPS->c_ps_src.rotate.angle_use, .angle_1 = v, .angle_2 = pPS->c_ps_src.rotate.angle_2 }); }
void G_PS_Setup::Rotate__angle_2(float v) { pPS->Set_Rotate(G_PS_Source::Rotate{ .angle_use = pPS->c_ps_src.rotate.angle_use, .angle_1 = pPS->c_ps_src.rotate.angle_1, .angle_2 = v, }); }
void G_PS_Setup::Animation__repeat(ui_zc v) { pPS->Set_Animation__repeat(G_PS_Source::Animation::Repaet(v)); }
void G_PS_Setup::Animation__tiles_per_second(float v) { pPS->Set_Animation__tiles_per_second(v); }
void G_PS_Setup::Animation__offset_from(ui_zc v) { pPS->Set_Animation__offset_from(G_PS_Source::Animation::OffsetFrom(v)); }
void G_PS_Setup::Animation__secs_offset_to_start_animation(float v) { pPS->Set_Animation__secs_offset_to_start_animation(v); }
void G_PS_Setup::Color__system_alpha(float v) { pPS->Set_Color__system_alpha(v); }
void G_PS_Setup::Color__appear_secs(float v) { pPS->Set_Color__appear_secs(v); }
void G_PS_Setup::Color__rgb_use(uint v) { pPS->Set_Color__rgb_use(G_PS_Source::Color::RGBUse(v)); }
void G_PS_Setup::Color__rgba_start(float r, float g, float b, float a) { pPS->Set_Color__rgba_start({ r, g, b, a }); }
void G_PS_Setup::Color__disappear_secs(float v) { pPS->Set_Color__disappear_secs(v); }
void G_PS_Setup::Color__rgba_end(float r, float g, float b, float a) { pPS->Set_Color__rgba_end({ r, g, b, a }); }
void G_PS_Setup::Collision__particle_radius(float v) { pPS->Set_Collision__particle_radius(v); }
void G_PS_Setup::Collision__set_life_time(ui_zc v) { pPS->Set_Collision__set_life_time(G_ParticleSystem::Collision::SetLifeTime(v)); }
void G_PS_Setup::Collision__move_speed(float v) { pPS->Set_Collision__move_speed(v / 100.f); }
#endif SETUP__G_PARTICLE_SYSTEM