#include "G_TParticle.h"

#include <ZC/File/ZC_File.h>
#include <System/G_RenderLevel.h>
#include <ZC/ZC__System.h>
#include <System/G_UpdaterLevels.h>

#include <vector>
#include <string>

G_TParticle::G_TParticle(ul_zc count)
    : ds(CreateDrawerSet(count)),
    ds_con(ds.MakeZC_DSController(0))
{
    ds_con.SetUniformsData(G_UN_unData, &unData);
    ds_con.SwitchToDrawLvl(ZC_RL_Default, ZC_DL_Drawing);

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
   unData[1][0] = upSP->test_params.life_start_sec;
}

G_TParticle::~G_TParticle()
{

}

ZC_DrawerSet G_TParticle::CreateDrawerSet(ul_zc count)
{
    std::vector<ZC_Vec4<float>> pos_and_life_time;
    pos_and_life_time.reserve(count);

    pos_and_life_time.emplace_back(ZC_Vec4<float>(0.f, 0.f, 2.f, 5.f));
    ZC_Buffer vbo(GL_ARRAY_BUFFER);
    vbo.GLNamedBufferStorage(pos_and_life_time.size() * sizeof(ZC_Vec4<float>), pos_and_life_time.data(), GL_DYNAMIC_STORAGE_BIT);

	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawArrays>(GL_POINTS, 0, count);

    ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ZC_ShPName::ShPN_Game_Flame);

	ZC_VAO vao;
	// vao.Config(pShPIS->vaoConfigData, vbo, nullptr, 0, 0);
	vao.Config(pShPIS->vaoConfigData, vbo, nullptr, count, 0);

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