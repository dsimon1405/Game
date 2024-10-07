#include "G_Window.h"

#include <ZC/Video/ZC_SWindow.h>
#include <ZC/Audio/ZC_Audio.h>
#include <ZC/Audio/ZC_Sounds.h>
#include <ZC/File/ZC_File.h>
#include <Sound/G_SoundName.h>
#include <ZC/GUI/ZC__GUI.h>
#include <Model/G_Models.h>
#include "G_DrawerCreator.h"    //  activate game render levels
#include <ZC/Objects/Text/ZC_Fonts.h>
#include "G_FontData.h"

G_Window::G_Window()
{
    LoadSounds();
        //  setup gui
    ZC__GUI::SetFontHeight(40);
        //  create window
    
    ZC_SWindow::MakeWindow(ZC_SWF__Multisampling_4 | ZC_SWF__Border | ZC_SWF__GUI, 800, 600, "SPHERUM");
    ZC_SWindow::SetMinSize(800, 600);
    if (config.GetConfigData().fuull_screen) ZC_SWindow::SetFullScreen(true);
    // ZC_SWindow::SetFPS(0);
    
    ZC_SWindow::NeedDrawFPS(true);
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

void G_Window::RunMainCycle()
{
    ZC_SWindow::RunMainCycle();
}

void G_Window::LoadSounds()
{
    ZC_FSPath path = ZC_FSPath(ZC_assetsDirPath).append("Game/sounds");

    // ZC_Sounds::LoadWAV(G_SN__sphere_move, "/home/dmitry/Загрузки/shpere_move_16S.wav");
    ZC_Sounds::LoadWAV(G_SN__sphere_move, ZC_FSPath(path).append("sphere/sphere_move.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_flight, ZC_FSPath(path).append("sphere/sphere_flight.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_jump, ZC_FSPath(path).append("sphere/sphere_jump.wav").string());
    ZC_Sounds::LoadWAV(G_SN__sphere_lands, ZC_FSPath(path).append("sphere/sphere_lands.wav").string());
}