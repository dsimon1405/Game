#pragma once

enum G_SoundName
{
    G_SN__main_theme,
        //  objects
    G_SN__star,
            //  sphere
    G_SN__sphere_move,
    G_SN__sphere_flight,
    G_SN__sphere_jump,
    G_SN__sphere_lands,
    G_SN__sphere_dmg_1,
    G_SN__sphere_dmg_2,
    G_SN__sphere_arson,

            //  platform
    G_SN__platform_activation,
    G_SN__platform_disapear,
    G_SN__platform_scale,
    G_SN__platform_win,
                //  wind
    G_SN__platform_wind_wind,
    G_SN__platform_wind_bubble,
                //  dmg
    G_SN__platform_dmg_make_dmg,
    G_SN__platform_dmg_load_dmg,

        //  gui
    G_SN__gui_start_timer,
    G_SN__gui_button_focused,
    G_SN__gui_button_clicked,
            //  win
    G_SN__gui_win_sound,
    G_SN__gui_win_transfer_time_points,

};