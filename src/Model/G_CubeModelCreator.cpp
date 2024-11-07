#include "G_CubeModelCreator.h"

#include <ZC/File/ZC_File.h>

G_ModelSet G_CubeModelCreator::LoadModel()
{       //  vbo
    std::vector<ZC_Quad> quads = GetCoords();
    std::vector<int> normals = GetNormals();
    std::vector<ushort> tex_coords = GetTexCoords();
    ZC_Buffer vbo(GL_ARRAY_BUFFER);
    auto quads_b_size = sizeof(ZC_Quad) * quads.size();
    auto normals_b_size = sizeof(int) * normals.size();
    auto tex_coords_b_size = sizeof(ushort) * tex_coords.size();
    vbo.GLNamedBufferStorage(quads_b_size + normals_b_size + tex_coords_b_size, quads.data(), GL_DYNAMIC_STORAGE_BIT);
    vbo.GLNamedBufferSubData(quads_b_size, normals_b_size, normals.data());
    vbo.GLNamedBufferSubData(quads_b_size + normals_b_size, tex_coords_b_size, tex_coords.data());

        //  ebo
    ulong elementsCount = 0;
    GLenum elementsType = 0;
    ZC_DA<uchar> elements = GetTriangleElements(elementsCount, elementsType, quads.size());

    ZC_Buffer ebo(GL_ELEMENT_ARRAY_BUFFER);
    ebo.GLNamedBufferData(elements.size, elements.Begin(), GL_STATIC_DRAW);
        //  draw
    auto upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, elementsCount, elementsType, 0);
        //  shProg
    typename ZC_ShProgs::ShPInitSet* pShPIS = ZC_ShProgs::Get(ShPN_Game_Sphere);
        //  vao
    ZC_VAO vao;
    vao.Config(pShPIS->vaoConfigData, vbo, &ebo, 0, quads.size() * 4);   //  vertices count in vbo (in one quad 4)
        //  buffers
    std::forward_list<ZC_Buffer> buffers;
    buffers.emplace_front(std::move(vbo));
    buffers.emplace_front(std::move(ebo));

        //	texture
    std::forward_list<ZC_TexturesSet> tex_sets;
        // retrieve the directory path of the filepath
    std::string tex_path = ZC_FSPath(ZC_assetsDirPath).append("Game/models/platform/stone.jpg").string();

    std::vector<ZC_Texture> textures;
    ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
    auto pNext_tex_name = texCreator.NextName();
    if (pNext_tex_name && *pNext_tex_name == ZC_TexSets::texColor)
    {
        texCreator.Add(ZC_Texture::LoadTexture2D(tex_path.c_str(), 0));
    }
    textures = texCreator.GetVector();
    tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = std::move(textures) });

    return G_ModelSet{ .model_name = G_MN__Platform_cube, .drawer_set = ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets)), .surfaces = Get_COSurfaces() };
}

ZC_DA<uchar> G_CubeModelCreator::GetTriangleElements(ulong& rElementsCount, GLenum& rElementsType, ulong quad_size)
{
    ulong quadsElementsCount = quad_size * 6;     //  6 elements in ebo on one quad
    rElementsCount = quadsElementsCount;  
    ulong verticesInVBO = quad_size * 4,     //  4 vertices in vbo on one quad
        storingTypeSize = 0;
    ZC_Buffer::GetElementsData(verticesInVBO - 1, storingTypeSize, rElementsType);
    ZC_DA<uchar> elements(storingTypeSize * rElementsCount);
    switch (storingTypeSize)
    {
    case 1: FillTriangleElements<uchar>(elements.pHead, elements.size, quadsElementsCount); break;
    case 2: FillTriangleElements<ushort>(reinterpret_cast<ushort*>(elements.pHead), elements.size / 2, quadsElementsCount); break;
    case 4: FillTriangleElements<uint>(reinterpret_cast<uint*>(elements.pHead), elements.size / 4, quadsElementsCount); break;
    }
    return elements;
}

std::vector<ZC_Quad> G_CubeModelCreator::GetCoords()
{
    return
        {
            {       //  near -Y
                {-1.f, -1.f, -1.f },    //  bl
                { 1.f, -1.f,  1.f },    //  tr
                {-1.f, -1.f,  1.f },    //  tl
                { 1.f, -1.f, -1.f }     //  br
            },
            {       //  left -X
                {-1.f,  1.f, -1.f },
                {-1.f, -1.f,  1.f },
                {-1.f,  1.f,  1.f },
                {-1.f, -1.f, -1.f }, 
            },
            {       //  far +Y
                { 1.f,  1.f, -1.f },
                {-1.f,  1.f,  1.f },
                { 1.f,  1.f,  1.f },
                {-1.f,  1.f, -1.f }
            },
            {       //  right +X
                { 1.f, -1.f, -1.f },
                { 1.f,  1.f,  1.f },
                { 1.f, -1.f,  1.f },
                { 1.f,  1.f, -1.f }
            },
            {       //  top +Z
                {-1.f, -1.f,  1.f },
                { 1.f,  1.f,  1.f },
                {-1.f,  1.f,  1.f },
                { 1.f, -1.f,  1.f }
            },
            {       //  bottom -Z
                { 1.f, -1.f, -1.f },
                {-1.f,  1.f, -1.f },
                { 1.f,  1.f, -1.f },
                {-1.f, -1.f, -1.f }
            }
        };
}

std::vector<int> G_CubeModelCreator::GetNormals()
{
    return
    {
        524288,
        524288,
        524288,
        524288,
        
        512,
        512,
        512,
        512,

        523264,
        523264,
        523264,
        523264,

        511,
        511,
        511,
        511,
        
        535822336,
        535822336,
        535822336,
        535822336,
        
        536870912,
        536870912,
        536870912,
        536870912,
    };
}

std::vector<ushort> G_CubeModelCreator::GetTexCoords()
{
    return
        {
            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,
            
            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,
            
            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,

            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,

            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,

            0, 0,
            65535, 65535,
            0, 65535,
            65535, 0,
        };
}

std::vector<ZC_CO_Surface<ZC_Vec3<float>>> G_CubeModelCreator::Get_COSurfaces()
{
    return
    {
        {
            std::vector<ZC_Vec3<float>>
                {       //  -Y
                    {-1.f, -1.f, -1.f },    //  bl
                    { 1.f, -1.f,  1.f },    //  tr
                    {-1.f, -1.f,  1.f },    //  tl
                    { 1.f, -1.f, -1.f }     //  br
                },
            ZC_Vec3<float>(0.f, -1.f, 0.f)
        },
        {
            {       //  -X
                {-1.f,  1.f, -1.f },
                {-1.f, -1.f,  1.f },
                {-1.f,  1.f,  1.f },
                {-1.f, -1.f, -1.f }, 
            },
            { -1.f, 0.f, 0.f }
        },
        {
            {       //  +Y
                { 1.f,  1.f, -1.f },
                {-1.f,  1.f,  1.f },
                { 1.f,  1.f,  1.f },
                {-1.f,  1.f, -1.f }
            },
            { 0.f, 1.f, 0.f }
        },
        {
            {       //  +X
                { 1.f, -1.f, -1.f },
                { 1.f,  1.f,  1.f },
                { 1.f, -1.f,  1.f },
                { 1.f,  1.f, -1.f }
            },
            { 1.f, 0.f, 0.f }
        },
        {
            {       //  +Z
                {-1.f, -1.f,  1.f },
                { 1.f,  1.f,  1.f },
                {-1.f,  1.f,  1.f },
                { 1.f, -1.f,  1.f }
            },
            { 0.f, 0.f, 1.f }
        },
        {
            {       //  -Z
                { 1.f, -1.f, -1.f },
                {-1.f,  1.f, -1.f },
                { 1.f,  1.f, -1.f },
                {-1.f, -1.f, -1.f }
            },
            { 0.f, 0.f, -1.f }
        },
    };
}