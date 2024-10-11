// #include "G_CubeMap.h"

// #include <System/G_RenderLevel.h>

// G_CubeMap::G_CubeMap()
//     : ds(CreateDrawerSet()),
//     dsCon(ds.MakeZC_DSController(0))
// {
//     dsCon.SwitchToDrawLvl(ZC_RL_Default, G_DL_CubeMap);
// }

// ZC_DrawerSet G_CubeMap::CreateDrawerSet()
// {       //  vbo
//     std::vector<float> vertices = GetCubeVretices();
// 	ZC_Buffer vbo(GL_ARRAY_BUFFER);
// 	vbo.GLNamedBufferStorage(vertices.size() * sizeof(float), vertices.data(), 0);
//         //  ebo
//     ulong elementsCount = 0;
//     GLenum elementsType;
//     ZC_DA<uchar> elements = ZC_Buffer::GetTriangleElements(elementsCount, elementsType, 6, 0);

// 	ZC_Buffer ebo(GL_ELEMENT_ARRAY_BUFFER);
// 	ebo.GLNamedBufferStorage(elements.size, elements.Begin(), 0);

//     auto upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, elementsCount, elementsType, 0);

// 		//	ShPInitSet
// 	typename ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ShPN_Game_CubeMap);

// 		//	vao
// 	ZC_VAO vao;
// 	vao.Config(pShPIS->vaoConfigData, vbo, &ebo, 0, 0);   //  vertices count in vbo (in one quad 4, in one triangle 3)

// 	std::forward_list<ZC_Buffer> buffers;
// 	buffers.emplace_front(std::move(vbo));
// 	buffers.emplace_front(std::move(ebo));

//     std::vector<std::string> paths
//         { 
//             { "/home/dmitry/Загрузки/skybox/right.jpg" },
//             { "/home/dmitry/Загрузки/skybox/left.jpg" },
//             { "/home/dmitry/Загрузки/skybox/front.jpg" },
//             { "/home/dmitry/Загрузки/skybox/back.jpg" },
//             { "/home/dmitry/Загрузки/skybox/top.jpg" },
//             { "/home/dmitry/Загрузки/skybox/bottom.jpg" },
//         };

//     std::vector<ZC_Texture> textures;
//     textures.reserve(1.f);
//     textures.emplace_back(ZC_Texture::LoadCubeMap(paths));

//     std::forward_list<ZC_TexturesSet> tex_sets;
//     tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = std::move(textures) });

// 	return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets));
// }

// std::vector<float> G_CubeMap::GetCubeVretices()
// {
//     return
//     {   //  look from the inside of the box
//             //  right +X
//          1.f,  1.f, -1.f,     //  bl
//          1.f, -1.f,  1.f,     //  tr
//          1.f,  1.f,  1.f,     //  tl
//          1.f, -1.f, -1.f,     //  br
//         //  1.f, -1.f, -1.f,     //  bl
//         //  1.f,  1.f,  1.f,     //  tr
//         //  1.f,  1.f, -1.f,     //  tl
//         //  1.f, -1.f,  1.f,     //  br
    
//             //  left -X
//         -1.f, -1.f, -1.f,
//         -1.f,  1.f,  1.f,
//         -1.f, -1.f,  1.f,
//         -1.f,  1.f, -1.f,
    
//             //  front +Y
//         -1.f,  1.f, -1.f,
//          1.f,  1.f,  1.f,
//         -1.f,  1.f,  1.f,
//          1.f,  1.f, -1.f,
    
//             //  back -Y
//          1.f, -1.f, -1.f,
//         -1.f, -1.f,  1.f,
//          1.f, -1.f,  1.f,
//         -1.f, -1.f, -1.f,
    
//             //  top +Z
//          1.f, -1.f,  1.f,
//         -1.f,  1.f,  1.f,
//          1.f,  1.f,  1.f,
//         -1.f, -1.f,  1.f,
    
//             //  bottom -Z
//         -1.f, -1.f, -1.f,
//          1.f,  1.f, -1.f,
//         -1.f,  1.f, -1.f,
//          1.f, -1.f, -1.f,
//     };
    


//     return {
//         // positions          
//         -1.0f,  1.0f, -1.0f,
//         -1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,

//         -1.0f, -1.0f,  1.0f,
//         -1.0f, -1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f, -1.0f,
//         -1.0f,  1.0f,  1.0f,
//         -1.0f, -1.0f,  1.0f,

//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,

//         -1.0f, -1.0f,  1.0f,
//         -1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f, -1.0f,  1.0f,
//         -1.0f, -1.0f,  1.0f,

//         -1.0f,  1.0f, -1.0f,
//          1.0f,  1.0f, -1.0f,
//          1.0f,  1.0f,  1.0f,
//          1.0f,  1.0f,  1.0f,
//         -1.0f,  1.0f,  1.0f,
//         -1.0f,  1.0f, -1.0f,

//         -1.0f, -1.0f, -1.0f,
//         -1.0f, -1.0f,  1.0f,
//          1.0f, -1.0f, -1.0f,
//          1.0f, -1.0f, -1.0f,
//         -1.0f, -1.0f,  1.0f,
//          1.0f, -1.0f,  1.0f
//     };
// }