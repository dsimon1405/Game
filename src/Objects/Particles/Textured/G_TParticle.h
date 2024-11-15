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


    G_TParticle(ul_zc count, float appear_circle_radius, float life_time_min, float life_time_max);
    ~G_TParticle();

    static float CalcLifeTime(float secs_min, float secs_max);
    static ZC_Vec2<float> CalcRandPosXYInCircle(float circle_radius);
    
// private:
    std::vector<ZC_Vec4<float>> pos_and_life_time;

    ZC_DrawerSet ds;
    ZC_DSController ds_con;

    ZC_EC ec_updater;

    ZC_Mat4<float> unData;

    ZC_DrawerSet CreateDrawerSet(ul_zc count, float appear_circle_radius, float life_time_min, float life_time_max);
    void Callback_Updater(float time);
};



#include <ZC/GUI/ZC_GUI__Window.h>
#include <ZC/GUI/Button/ZC_GUI__ButtonNumberText.h>
struct SetupParticle
{
    struct TestParams
    {
        float tiles_per_second = 7.f;
        float width = 3.f;
        float height = 3.f;
        float appear_secs = 1.f;
        float pos_x = 0.f;
        float pos_y = 0.f;
        float pos_z = 2.f;
        float life_secs_min = 3.f;
        float life_secs_max = 5.f;
        float circle_radius = 0.5f;
        float particles_count = 10.f;
    } test_params;

    ZC_GUI__WinImmutable win;
    ZC_GUI__ButtonNumberText<float> bnt_tiles_per_second;
    ZC_GUI__ButtonNumberText<float> bnt_particle_width;
    ZC_GUI__ButtonNumberText<float> bnt_particle_height;
    ZC_GUI__ButtonNumberText<float> bnt_life_start_sec;
    ZC_GUI__ButtonNumberText<float> bnt_pos_x;
    ZC_GUI__ButtonNumberText<float> bnt_pos_y;
    ZC_GUI__ButtonNumberText<float> bnt_pos_z;
    ZC_GUI__ButtonNumberText<float> bnt_min_life_secs;
    ZC_GUI__ButtonNumberText<float> bnt_max_life_secs;
    ZC_GUI__ButtonNumberText<float> bnt_circle_radius;
    ZC_GUI__ButtonNumberText<float> bnt_particles_count;

    SetupParticle()
        : win(ZC_WOIData(300.f, 300.f, 0.f, 0.f, ZC_WOIF__X_Right_Pixel | ZC_WOIF__Y_Top_Pixel), ZC_GUI_WF__NeedDraw | ZC_GUI_WF__Scrollable),
        bnt_tiles_per_second(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.tiles_per_second, 0.f, 1000.f, 1.f, 1.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Tiles_per_second, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"tiles_per_second", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_particle_width(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.width, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::width, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"width", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_particle_height(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.height, 0.01f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Height, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"height", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_life_start_sec(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.appear_secs, 0.f, 100.f, 0.01f, 0.1f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"appear_secs", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_pos_x(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.pos_x, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"pos_x", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_pos_y(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.pos_y, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"pos_y", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_pos_z(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.pos_z, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"pos_z", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_min_life_secs(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_secs_min, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"life_secs_min", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_max_life_secs(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_secs_max, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"life_secs_max", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_circle_radius(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_secs_max, 0.f, 100.f, 0.1f, 1.f, 2, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"circle_radius", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0))),
        bnt_particles_count(ZC_GUI_ButtonNumber<float>(0.f, 0.f, test_params.life_secs_max, 1.f, 10000.f, 1.f, 2.f, 0, ZC_GUI_TextAlignment::Right, { &SetupParticle::Appear_secs, this }, nullptr), ZC_GUI_TextForButton(ZC_GUI_TFB_Indent(0.f, ZC_GUI_TFB_Indent_Location::OutOfButtonLeft), L"particles_count", true, 0, ZC_GUI_TextAlignment::Left, ZC_GUI_TFB_Colors(ZC_PackColorUCharToUInt_RGB(230, 230, 230), 0)))
    {
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_tiles_per_second.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_particle_width.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_particle_height.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_life_start_sec.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_pos_x.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_pos_y.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_pos_z.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_min_life_secs.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_max_life_secs.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_circle_radius.GetObj() }));
        win.AddRow(ZC_GUI_Row(ZC_GUI_RowParams(0.f, ZC_GUI_RowParams::Right, 5.f, 0.f), { bnt_particles_count.GetObj() }));
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
    void Appear_secs(float v)
    {
        G_TParticle::pStatic->unData[1][0] = v;
    }
    void Pos_x(float v)
    {
        G_TParticle::pStatic->unData[1][1] = v;
    }
    void Pos_y(float v)
    {
        G_TParticle::pStatic->unData[1][2] = v;
    }
    void Pos_z(float v)
    {
        G_TParticle::pStatic->unData[1][3] = v;
    }
    void Min_life_secs(float v)
    {
        for (ZC_Vec4<float>& plt : G_TParticle::pStatic->pos_and_life_time) plt[3] = G_TParticle::CalcLifeTime(v, test_params.life_secs_max);
        G_TParticle::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_TParticle::pStatic->pos_and_life_time.size(), &v);
    }
    void Max_life_secs(float v)
    {
        for (ZC_Vec4<float>& plt : G_TParticle::pStatic->pos_and_life_time) plt[3] = G_TParticle::CalcLifeTime(test_params.life_secs_min, v);
        G_TParticle::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_TParticle::pStatic->pos_and_life_time.size(), &v);
    }
    void Circle_radius(float v)
    {
        for (ZC_Vec4<float>& plt : G_TParticle::pStatic->pos_and_life_time)
        {
            ZC_Vec2<float> pos_xy = G_TParticle::CalcRandPosXYInCircle(v);
            plt = ZC_Vec4<float>(pos_xy[0], pos_xy[1], 0.f, plt[3]);
        }
        G_TParticle::pStatic->ds.buffers.front().GLNamedBufferSubData(0ll, sizeof(ZC_Vec4<float>) * G_TParticle::pStatic->pos_and_life_time.size(), &v);
    }
    void Particles_count(float v)
    {
        std::vector<ZC_Vec4<float>> pos_and_life_time;
        pos_and_life_time.reserve(v);
        for (ZC_Vec4<float>& plt : G_TParticle::pStatic->pos_and_life_time)     //  copy existing data
        {
            if (pos_and_life_time.capacity() == pos_and_life_time.size()) break;
            pos_and_life_time.emplace_back(plt);
        }
        if (pos_and_life_time.capacity() != pos_and_life_time.size())   //  if existing data not enought, create new
        {
            for (ul_zc i = pos_and_life_time.size(); i < pos_and_life_time.capacity(); ++i)
            {
                ZC_Vec2<float> pos_xy = G_TParticle::CalcRandPosXYInCircle(test_params.circle_radius);
                pos_and_life_time.emplace_back(ZC_Vec4<float>(pos_xy[0], pos_xy[1], 0.f, G_TParticle::CalcLifeTime(test_params.life_secs_min, test_params.life_secs_max)));
            }
        }
        
        ul_zc particles_count = pos_and_life_time.size();
        ZC_Buffer vbo(GL_ARRAY_BUFFER);
        vbo.GLNamedBufferStorage(particles_count * sizeof(ZC_Vec4<float>), pos_and_life_time.data(), GL_DYNAMIC_STORAGE_BIT);

        ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, particles_count);

        ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

        ZC_VAO vao;
        // vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, 0);
        vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, particles_count);

        G_TParticle::pStatic->ds.upGLDraw = std::move(upDraw);
        G_TParticle::pStatic->ds.vao = std::move(vao);
        G_TParticle::pStatic->ds.buffers.front() = std::move(vbo);

        G_TParticle::pStatic->ds_con.pGLDraw = G_TParticle::pStatic->ds.upGLDraw.Get();
        G_TParticle::pStatic->ds_con.pVAO = &(G_TParticle::pStatic->ds.vao);
    }
};