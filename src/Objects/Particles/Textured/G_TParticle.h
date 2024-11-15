#pragma once

#include <ZC/ZC_Types.h>
#include <ZC/Tools/Math/ZC_Vec3.h>
#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>
#include <ZC/Tools/Math/ZC_Mat4.h>

struct SetupParticle;

class G_TParticle
{
public:
    static inline G_TParticle* pStatic;
    ZC_uptr<SetupParticle> upSP;


    G_TParticle(ul_zc count);
    ~G_TParticle();

// private:
    ZC_DrawerSet ds;
    ZC_DSController ds_con;

    ZC_EC ec_updater;

    ZC_Mat4<float> unData;

    ZC_DrawerSet CreateDrawerSet(ul_zc count);
    void Callback_Updater(float time);
};



#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
struct SetupParticle
{
    struct TestParams
    {
        float tiles_per_second = 2.f;
        float width = 0.3f;
        float height = 0.3f;
        float life_time_sec = 5.f;
        float life_start_sec = 1.f;
    } test_params;

    ZC_GUI__WinImmutable win;
    ZC_GUI__ButtonNumberText<float> bnt_tiles_per_second;
    ZC_GUI__ButtonNumberText<float> bnt_particle_width;
    ZC_GUI__ButtonNumberText<float> bnt_particle_height;
    ZC_GUI__ButtonNumberText<float> bnt_life_time_sec;
    ZC_GUI__ButtonNumberText<float> bnt_life_start_sec;

    SetupParticle()
        : win(ZC_WOIData(330.f, 200.f, 0.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
        bnt_tiles_per_second(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.tiles_per_second, 0.f, 1000.f, 1.f, 1.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Tiles_per_second, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"tiles_per_second", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_particle_width(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.width, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::width, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"width", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_particle_height(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.height, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Height, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"height", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_life_time_sec(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_time_sec, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Life_time_sec, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"life_time_sec", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_life_start_sec(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_start_sec, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Life_start_sec, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"life_start_sec", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0)))
    {
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_tiles_per_second.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_particle_width.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_particle_height.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_life_time_sec.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_life_start_sec.GetObj() }));
    }

    void Tiles_per_second(float v)
    {
        G_TParticle::pStatic->unData[0][1] = v;
    }
    void width(float v)
    {
        G_TParticle::pStatic->unData[0][2] = v;
    }
    void Height(float v)
    {
        G_TParticle::pStatic->unData[0][3] = v;
    }
    void Life_time_sec(float v)
    {
        G_TParticle::pStatic->unData[1][0] = v;
    }
    void Life_start_sec(float v)
    {
        G_TParticle::pStatic->ds.buffers.front().GLNamedBufferSubData(sizeof(ZC_Vec3<float>), sizeof(float), &v);
    }
};