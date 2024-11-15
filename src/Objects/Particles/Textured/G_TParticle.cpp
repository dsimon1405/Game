#include "G_TParticle.h"

#include <ZC/File/ZC_File.h>
#include <System/G_RenderLevel.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>
#include <ZC/Tools/ZC_Random.h>

#include <vector>
#include <string>

G_TParticle::G_TParticle(ul_zc count, float appear_circle_radius, float life_secs_min, float life_secs_max)
    : pos_and_life_time{},
    ds(CreateDrawerSet(count, appear_circle_radius, life_secs_min, life_secs_max)),
    ds_con(ds.MakeZC_DSController(0))
{
    ds_con.SetUniformsData(G_UN_unData, &unData);
    ds_con.SwitchToDrawLvl(ZC_RL_Default, G_DL_AlphaBlending_PlatformDisappear);

    ec_updater = ZC__Updater::Connect({ &G_TParticle::Callback_Updater, this }, G_UL__game_particles);
    
    pStatic = this;
    upSP = new SetupParticle();
    /*
    data[0].x - cur time in seconds
    data[0].y - tiles per second
    data[0].z - particle widht
    data[0].w - particle height

    data[1].x - appear/disappear seconds. other time particle have alpha 1.f
    */
    unData[0][1] = upSP->test_params.tiles_per_second;
    unData[0][2] = upSP->test_params.width;
    unData[0][3] = upSP->test_params.height;
    
    unData[1][0] = upSP->test_params.appear_secs;
    unData[1][1] = upSP->test_params.pos_x;        
    unData[1][2] = upSP->test_params.pos_y;        
    unData[1][3] = upSP->test_params.pos_z;        
}

G_TParticle::~G_TParticle()
{
    ec_updater.Disconnect();
}

float G_TParticle::CalcLifeTime(float secs_min, float secs_max)
{
    return f_zc(ZC_Random::GetRandomInt(secs_min * 100.f, secs_max * 100.f)) / 100.f;
}

ZC_Vec2<float> G_TParticle::CalcRandPosXYInCircle(float circle_radius)
{
    float dir_x = f_zc(ZC_Random::GetRandomInt(-100, 100)) / 100.f;
    float dir_y = f_zc(ZC_Random::GetRandomInt(-100, 100)) / 100.f;
    float dist = f_zc(ZC_Random::GetRandomInt(0, circle_radius * 100.f)) / 100.f;
    return ZC_Vec::MoveByLength({}, { dir_x, dir_y }, dist);
}

ZC_DrawerSet G_TParticle::CreateDrawerSet(ul_zc count, float appear_circle_radius, float life_secs_min, float life_secs_max)
{
    pos_and_life_time.reserve(count);

    for (ul_zc i = 0ull; i < count; ++i)
    {
        ZC_Vec2<float> pos_xy = CalcRandPosXYInCircle(appear_circle_radius);
        pos_and_life_time.emplace_back(ZC_Vec4<float>(pos_xy[0], pos_xy[1], 0.f, CalcLifeTime(life_secs_min, life_secs_max)));
    }
    
    ZC_Buffer vbo(GL_ARRAY_BUFFER);
    vbo.GLNamedBufferStorage(pos_and_life_time.size() * sizeof(ZC_Vec4<float>), pos_and_life_time.data(), GL_DYNAMIC_STORAGE_BIT);

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, count);

    ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

	ZC_VAO vao;
	// vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, 0);
	vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, count);

	std::forward_list<ZC_Buffer> buffers;
	buffers.emplace_front(std::move(vbo));

	std::forward_list<ZC_TexturesSet> tex_sets;
    ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
    texCreator.Add(ZC_Texture::LoadTexture2D(ZC_FSPath(ZC_assetsDirPath).append("Game/textures/flame.png").string().c_str(), 0, GL_REPEAT, GL_REPEAT));
    tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = texCreator.GetVector() });

    return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets));
}

void G_TParticle::Callback_Updater(float time)
{
    static ZC_Clock cl;
    G_TParticle::pStatic->unData[0][0] = (float)cl.Time<ZC_Nanoseconds>() / 1000000000.f;   //  to seconds
}
