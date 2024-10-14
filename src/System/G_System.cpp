#include "G_System.h"

#include <ZC/Video/ZC_SWindow.h>
#include <ZC/File/ZC_File.h>
#include <ZC/GUI/ZC__GUI.h>
#include <Model/G_Models.h>
#include "G_DrawerCreator.h"    //  activate game render levels
#include <ZC/Objects/Text/ZC_Fonts.h>
#include "G_FontData.h"
#include <ZC/Audio/ZC_Audio.h>
#include <ZC/Audio/ZC_Sounds.h>
#include <Sound/G_SoundName.h>
#include <ZC/GUI/Backend/Config/ZC_GUI_IconUV.h>
#include <ZC/GUI/Backend/Config/ZC_GUI_Colors.h>

G_System::G_System()
{
    LoadSounds();
    SetUpGUI();
        //  create window
    ZC_SWindow::MakeWindow(ZC_SWF__Multisampling_4 | ZC_SWF__Border | ZC_SWF__GUI, 800, 600, "SPHERUM");
    ZC_SWindow::SetMinSize(800, 600);
    if (config.GetConfigData().fuull_screen) ZC_SWindow::SetFullScreen(true);
    ZC_SWindow::GlEnablePointSize();    //  particle system
    // ZC_SWindow::SetFPS(0);
    
    // ZC_SWindow::NeedDrawFPS(true);
    ZC_SWindow::SetFPSTimeMeasure(ZC_FPS_TM__Seconds);
    ZC_SWindow::GlClearColor(0.5, 0.5, 0.5, 1.f);

    config.CreateGUI();
        //      models
    G_Models::LoadModels();
        //  fonts for ZC_Text class, not ZC_GUI (gui load for them self)
    G_FontData::LoadFonts();
        //  open audio stream
    ZC_Audio::OpenAudioStream(ZC_AudioSet(ZC_Sounds::GetSound(0)->GetAudioSet()));
}

void G_System::RunMainCycle()
{
    ZC_SWindow::RunMainCycle();
}

void G_System::LoadSounds()
{
    ZC_FSPath path = ZC_FSPath(ZC_assetsDirPath).append("Game/sounds");
    
    //  load sounds
    ZC_Sounds::LoadWAV(G_SN__main_theme, ZC_FSPath(path).append("main_theme.wav").string());
        //  objects
            //  start
    ZC_Sounds::LoadWAV(G_SN__star, ZC_FSPath(path).append("objects/star.wav").string());
            //  sphere
    ZC_Sounds::LoadWAV(G_SN__sphere_move, ZC_FSPath(path).append("objects/sphere/sphere_move.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_flight, ZC_FSPath(path).append("objects/sphere/sphere_flight.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_jump, ZC_FSPath(path).append("objects/sphere/sphere_jump.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_lands, ZC_FSPath(path).append("objects/sphere/sphere_lands.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_dmg_1, ZC_FSPath(path).append("objects/sphere/sphere_dmg_1.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_dmg_2, ZC_FSPath(path).append("objects/sphere/sphere_dmg_2.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_arson, ZC_FSPath(path).append("objects/sphere/sphere_arson.wav").string());
            //  platform
    ZC_Sounds::LoadWAV(G_SN__platform_activation, ZC_FSPath(path).append("objects/platforms/platform_activation.wav").string());
    ZC_Sounds::LoadWAV(G_SN__platform_disapear, ZC_FSPath(path).append("objects/platforms/platform_disapear.wav").string());
    ZC_Sounds::LoadWAV(G_SN__platform_scale, ZC_FSPath(path).append("objects/platforms/platform_scale.wav").string());
    ZC_Sounds::LoadWAV(G_SN__platform_wind, ZC_FSPath(path).append("objects/platforms/platform_wind.wav").string());
    ZC_Sounds::LoadWAV(G_SN__platform_win, ZC_FSPath(path).append("objects/platforms/platform_win.wav").string());
                //  dmg
    ZC_Sounds::LoadWAV(G_SN__platform_dmg_make_dmg, ZC_FSPath(path).append("objects/platforms/dmg/platform_dmg_make_dmg.wav").string());
    ZC_Sounds::LoadWAV(G_SN__platform_dmg_load_dmg, ZC_FSPath(path).append("objects/platforms/dmg/platform_dmg_load_dmg.wav").string());

        //  gui
    ZC_Sounds::LoadWAV(G_SN__gui_start_timer, ZC_FSPath(path).append("gui/start_timer.wav").string());
    ZC_Sounds::LoadWAV(G_SN__gui_button_focused, ZC_FSPath(path).append("gui/button_focused.wav").string());
    ZC_Sounds::LoadWAV(G_SN__gui_button_clicked, ZC_FSPath(path).append("gui/open_window.wav").string());
            //  win
    ZC_Sounds::LoadWAV(G_SN__gui_win_sound, ZC_FSPath(path).append("gui/win/win_sound.wav").string());
    ZC_Sounds::LoadWAV(G_SN__gui_win_transfer_time_points, ZC_FSPath(path).append("gui/win/win_transfer_time_points.wav").string());
}

void G_System::SetUpGUI()
{
    ZC__GUI::SetFontHeight(40);
    ZC__GUI::SetPathToCustomIconTexture(ZC_FSPath(ZC_assetsDirPath).append("Game/textures/game_icons.png").string());

    static const float icon_tex_width = 800.f;
    static const float icon_tex_height = 900.f;
    ZC_GUI_IconUV::quad = ZC_GUI_UV{ .bl = { 0.f, 101.f / icon_tex_height }, .tr = { 1.f, 1.f } };;
    ZC_GUI_IconUV::button = ZC_GUI_UV{ .bl = { 0.f, 0.f }, .tr = { 399.f / icon_tex_width, 100.f / icon_tex_height } };
    ZC_GUI_IconUV::arrow_down = ZC_GUI_UV{ .bl = { 502.f / icon_tex_width, 0.f }, .tr = { 602.f / icon_tex_width, 100.f / icon_tex_height } };
    ZC_GUI_IconUV::arrow_right = ZC_GUI_UV{ .bl = { 602.f / icon_tex_width, 0.f }, .tr = { 702.f / icon_tex_width, 100.f / icon_tex_height } };

    //  Widnow
    ZC_GUI_Colors::window = 0;
    ZC_GUI_Colors::window_focused = 0;
        //  text
    ZC_GUI_Colors::window_text = ZC_PackColorUCharToUInt_RGB(230, 230, 230);
        //  Button (for Window background)
    ZC_GUI_Colors::button = ZC_PackColorUCharToUInt_RGB(10, 10, 10);
    ZC_GUI_Colors::button_under_cursor = ZC_PackColorUCharToUInt_RGB(40, 40, 40);
    ZC_GUI_Colors::button_pressed = ZC_PackColorUCharToUInt_RGB(70, 70, 70);
    ZC_GUI_Colors::button_not_active = 0;
    ZC_GUI_Colors::button_text = 0;
    ZC_GUI_Colors::button_text_not_active = ZC_PackColorUCharToUInt_RGB(100, 100, 100);

    //  CheckBox
    ZC_GUI_Colors::checkBox_button = 0;
    ZC_GUI_Colors::checkBox_button_under_cursor = ZC_PackColorUCharToUInt_RGB(20, 20, 20);
    ZC_GUI_Colors::checkBox_button_pressed = ZC_PackColorUCharToUInt_RGB(50, 50, 50);
        //  arrow
    ZC_GUI_Colors::checkBox_arrow = 0;
        //  text
    ZC_GUI_Colors::checkBox_text = ZC_GUI_Colors::window_text;

    //  ButtonNumber
        //  button
    ZC_GUI_Colors::number_button = ZC_GUI_Colors::button;
    ZC_GUI_Colors::number_button_under_cursor = ZC_GUI_Colors::button_under_cursor;
    ZC_GUI_Colors::number_button_pressed = ZC_GUI_Colors::button_pressed;
        //  text
    ZC_GUI_Colors::number_text = ZC_GUI_Colors::window_text;
        //  arrows
    ZC_GUI_Colors::number_arrow = 0;
    ZC_GUI_Colors::number_arrow_under_cursor = ZC_PackColorUCharToUInt_RGB(10, 10, 10);
    ZC_GUI_Colors::number_arrow_pressed = ZC_PackColorUCharToUInt_RGB(30, 30, 30);

    //  DropDownSwitch
    ZC_GUI_Colors::dropDownSwitch_button = ZC_PackColorUCharToUInt_RGB(40, 40, 40);
    ZC_GUI_Colors::dropDownSwitch_button_under_cursor = ZC_PackColorUCharToUInt_RGB(60, 60, 60);
    ZC_GUI_Colors::dropDownSwitch_button_pressed = ZC_PackColorUCharToUInt_RGB(100, 100, 100);
    ZC_GUI_Colors::dropDownSwitch_text = 0;
    ZC_GUI_Colors::dropDownSwitch_arrow = 0;
}