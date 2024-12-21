#include "G_ModelLoader.h"

#include <ZC/File/ZC_File.h>
#include <ZC/Tools/Container/ZC_ContFunc.h>

#ifdef G_NewV
#include <fstream>

// void G_ModelLoader::WriteToFile(G_ModelName model_name, const std::vector<Vertex>& verts, const std::vector<ZC_CO_Surface<ZC_Vec3<float>>>& surfs)
// {
// 	/*
// 	Structure of .zcm file:
// 		size	-	value
	
// 			gpu data
// 	- 8 bytes - number of structs Vertex in byte size
// 	- byte size from previous line value - Vertex data
	
// 			collision data
// 	- 8 bytes - amount of structs ZC_CO_Surface<ZC_Vec3<float>> (amount, not size in bytes!)
// 		The following lines are the structure ZC_CO_Surface<ZC Vec3<float>>. Will repeat the number of times from the previous line
// 	- 12 bytes - ZC_CO_Surface<ZC Vec3<float>>.normal
// 	- 8 bytes - number of points in vector ZC_CO_Surface<ZC Vec3<float>>.points
// 		the next line is repeated the number of times from the previous line
// 	- 12 bytes - point
// 	*/
// 	std::ofstream ofs(model_name == G_MN__SphereMarble ? "sphere.zcm" : "cylinder.zcm", std::ios::binary);
// 	assert(ofs.is_open());
// 		//	vertices for gpu
// 	ul_zc verts_bytes = verts.size() * sizeof(Vertex);		//	byte size
// 	ofs.write(reinterpret_cast<char*>(&verts_bytes), sizeof(verts_bytes));	//	8 bytes
// 	ofs.write(reinterpret_cast<const char*>(verts.data()), sizeof(Vertex) * verts.size());

// 		//	surfaces for collistion
// 	ul_zc surfs_count = surfs.size();
// 	ofs.write(reinterpret_cast<char*>(&surfs_count), sizeof(surfs_count));
// 	for (const ZC_CO_Surface<ZC_Vec3<float>>& surf : surfs)
// 	{
// 		ofs.write(reinterpret_cast<const char*>(&surf.normal), sizeof(surf.normal));
		
// 		ul_zc points_count = surf.points.size();
// 		ofs.write(reinterpret_cast<char*>(&points_count), sizeof(points_count));
// 		ofs.write(reinterpret_cast<const char*>(surf.points.data()), points_count * sizeof(ZC_Vec3<float>));
// 	}

// 	ofs.close();
// }

G_ModelSet G_ModelLoader::LoadModel(G_ModelName model_name)
{
	/*
	Structure of .zcm file:
		size	-	value
	
			gpu data
	- 8 bytes - number of structs Vertex in byte size
	- byte size from previous line value - Vertex data
	
			collision data
	- 8 bytes - amount of structs ZC_CO_Surface<ZC_Vec3<float>> (amount, not size in bytes!)
		The following lines are the structure ZC_CO_Surface<ZC Vec3<float>>. Will repeat the number of times from the previous line
	- 12 bytes - ZC_CO_Surface<ZC Vec3<float>>.normal
	- 8 bytes - number of points in vector ZC_CO_Surface<ZC Vec3<float>>.points
		the next line is repeated the number of times from the previous line
	- 12 bytes - point
	*/
	std::ifstream ifs(ZC_FSPath(ZC_assetsDirPath).append(model_name == G_MN__SphereMarble ? "Game/models/sphere/sphere.zcm" : "Game/models/platform/cylinder.zcm").string().c_str(), std::ios::binary);
	assert(ifs.is_open());
		//	vertices for gpu
		//	read size of the Vertex array
	ul_zc verts_byte_size = 0;
	ifs.read(reinterpret_cast<char*>(&verts_byte_size), sizeof(verts_byte_size));
	assert(verts_byte_size % sizeof(Vertex) == 0);	//	byte size not equal strucs Vertex type
		//	read Vertex data
	ZC_DA<char> verts(verts_byte_size);
	ifs.read(verts.pHead, verts_byte_size);

		//	surfaces for collistion
	ul_zc surfs_count = 0;
	ifs.read(reinterpret_cast<char*>(&surfs_count), sizeof(surfs_count));
	std::vector<ZC_CO_Surface<ZC_Vec3<float>>> surfs;
	surfs.reserve(surfs_count);
	for (ul_zc i = 0; i < surfs_count; ++i)
	{
			//	normal
		ZC_Vec3<float> normal;
		ifs.read(reinterpret_cast<char*>(&normal), sizeof(normal));
			//	points
		ul_zc points_count = 0;
		ifs.read(reinterpret_cast<char*>(&points_count), sizeof(points_count));
		std::vector<ZC_Vec3<float>> points(points_count);
		for (ZC_Vec3<float>& p : points)
			ifs.read(reinterpret_cast<char*>(&p), sizeof(p));
		
		surfs.emplace_back(std::move(points), normal);
	}
	
	return G_ModelSet{ .model_name = model_name, .drawer_set = CreateDrawerSet(model_name, std::move(verts)), .surfaces = std::move(surfs) };
}

ZC_DrawerSet G_ModelLoader::CreateDrawerSet(G_ModelName model_name, ZC_DA<char>&& verts)
{
	ZC_Buffer vbo(GL_ARRAY_BUFFER);
	vbo.GLNamedBufferStorage(verts.size, verts.pHead, 0u);
	
	GLenum elementsType = 0;
	ul_zc elements_count = 0;
		//	draw order triangles of the quad: first(tl, br, bl), second(tl, tr, bl). GetTriangleElements() in description have another corners order (bl, tr, tl	bl, br, tr). But in both cases we will get order of indexes for first quad (0, 1, 2,  0, 3, 2) and so on for other quads...
	ZC_DA<uchar> elements = ZC_Buffer::GetTriangleElements(elements_count, elementsType, verts.size / sizeof(Vertex) / 4, 0);
	
	ZC_Buffer ebo(GL_ELEMENT_ARRAY_BUFFER);
	ebo.GLNamedBufferStorage(elements.size, elements.Begin(), 0u);
		
		//	glDraw
	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, elements_count, elementsType, 0u);

		//	ShPInitSet
	typename ZC_ShProgs::ShPInitSet* pShPIS =
		model_name == G_MN__SphereMarble || model_name == G_MN__Platform_cylinder_black ? ZC_ShProgs::Get(ShPN_Game_Sphere) 
		: nullptr;

		//	vao
	ZC_VAO vao;
	vao.Config(pShPIS->vaoConfigData, vbo, &ebo, 0, 0);

	std::forward_list<ZC_Buffer> buffers;
	buffers.emplace_front(std::move(vbo));
	buffers.emplace_front(std::move(ebo));

		//	texture
	std::forward_list<ZC_TexturesSet> tex_sets;
	ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
	texCreator.Add(ZC_Texture::LoadTexture2D(ZC_FSPath(ZC_assetsDirPath).append(model_name == G_MN__SphereMarble ? "Game/models/sphere/cube_map.jpg" : "Game/models/platform/marble_black.jpg").string().c_str(), 0, false, GL_REPEAT, GL_REPEAT));
	
	tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = texCreator.GetVector() });

	return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets));
}

#else

#include "G_Assimp_ZC_Converter.h"
#include <assimp/postprocess.h>



// #include <ZC/Tools/Time/ZC_Timer.h>
G_ModelSet G_ModelLoader::LoadModel(G_ModelName model_name)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	std::string model_path = GetPath(model_name);
	
    // static ZC_Timer t1(ZC_TR__repeats, 4.0, ZC_TRO__seconds_total, "assimp load");
    // t1.StartPoint();
	const aiScene* pScene = importer.ReadFile(model_path, 0);
	// t1.EndPoint();
	// const aiScene* pScene = importer.ReadFile(model_path, aiProcess_Triangulate );

	// check for errors
	if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) // if is Not Zero
		assert(false);

	// process ASSIMP's root pNode recursively
	return ProcessNode(model_name, pScene->mRootNode, pScene, model_path);
}

std::string G_ModelLoader::GetPath(G_ModelName model_name)
{
	static ZC_FSPath model_dir_path = ZC_FSPath(ZC_assetsDirPath).append("Game/models");
	switch (model_name)
	{
	case G_MN__SphereMarble: return ZC_FSPath(model_dir_path).append("sphere/sphere_marble.dae").string();
	case G_MN__SphereStar: return ZC_FSPath(model_dir_path).append("sphere/sphere_star.dae").string();
	case G_MN__SphereMap: return ZC_FSPath(model_dir_path).append("sphere/sphere_marble.dae").string();
	case G_MN__Platform_cylinder_black: return ZC_FSPath(model_dir_path).append("platform/cylinder_black.dae").string();
	default: assert(false); return {};
	}
}

G_ModelSet G_ModelLoader::ProcessNode(G_ModelName model_name, aiNode* pNode, const aiScene* pScene, const std::string& path)
{
	// aiNode* f = pNode->mChildren[0];
	// aiNode* s = pNode->mChildren[1];
	// std::vector<std::vector<int>> vs;
	// aiNode* pChild = pNode->mChildren[0];
	// uint element_index = 0;
	// for (uint q = 0; q < pChild->mNumMeshes; ++q)
	// {
	// 	vs.emplace_back();
	// 	aiMesh* mesh = pScene->mMeshes[pChild->mMeshes[q]];
	// 	for (uint i = 0, element_index = 0; i < mesh->mNumFaces; i++)
	// 	{
	// 		aiFace& face = mesh->mFaces[i];
	// 		for (uint j = 0; j < face.mNumIndices; j++)
	// 		{
	// 			vs[q].emplace_back(face.mIndices[j]);
	// 			++element_index;
	// 		}
	// 	}
	// }

	size_t draw_index = 0;
	size_t collision_index = 0;
	for (size_t i = 0; i < pNode->mNumChildren; ++i)
	{
		char first_letter = pNode->mChildren[i]->mName.data[0];
		if (first_letter == 'D') draw_index = i;	//	draw model
		else if (first_letter == 'C') collision_index = i;	//	collision model
	}

	std::vector<ZC_CO_Surface<ZC_Vec3<float>>> co_surfs;
	if (model_name != G_MN__SphereMap && model_name != G_MN__SphereStar) co_surfs = LoadCollisionSurfaces(pNode->mChildren[collision_index], pScene, model_name);	
	
	return G_ModelSet{ .model_name = model_name, .drawer_set = CreateDrawerSet(pNode->mChildren[draw_index], pScene, path,
		(model_name == G_MN__SphereMarble || model_name == G_MN__SphereMap), model_name == G_MN__SphereMap, model_name),
		.surfaces = std::move(co_surfs) };

	// static const aiMatrix4x4 model_one;
	// // process each mesh located at the current pNode
	// for(unsigned int i = 0; i < pNode->mNumMeshes; i++)
	// {
	// 	// the pNode object only contains indices to index the actual objects in the pScene. 
	// 	// the pScene contains all the data, pNode is just to keep stuff organized (like relations between nodes).
	// 	aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
	// 	return CreateDrawerSet(mesh, pScene, model_one != pNode->mTransformation ? &pNode->mTransformation : nullptr, path);
	// }
	// // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	// for(unsigned int i = 0; i < pNode->mNumChildren; i++)
	// {
	// 	return ProcessNode(pNode->mChildren[i], pScene, path);
	// }

	// assert(false);	//	must not be reached
	// return ZC_DrawerSet(nullptr, ZC_VAO(), nullptr, {});
}

std::vector<ZC_CO_Surface<ZC_Vec3<float>>> G_ModelLoader::LoadCollisionSurfaces(aiNode *pNode, const aiScene *pScene, G_ModelName model_name)
{		
		//	get model matrix if it is
	static const aiMatrix4x4 model_one;
	aiMatrix4x4* pModel = model_one != pNode->mTransformation ? &pNode->mTransformation : nullptr;
	ZC_Mat4<float> model = pModel ? G_Assimp_ZC_Converter::GetMat4(*pModel) : ZC_Mat4<float>(1.f);
		//	get mesh (for collision models only one)
	aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[0]];;
		//	on case if have model matrix
	auto lamb_CalculateVector = [pModel, &model](const aiVector3D& v)
	{
		if (pModel)
		{
			ZC_Vec4<float> pos = model * ZC_Vec4<float>(G_Assimp_ZC_Converter::GetVec3(v), 1.f);
			return ZC_Vec3<float>(pos[0], pos[1], pos[2]);
		}
		else return G_Assimp_ZC_Converter::GetVec3(v);
	};
	
	std::vector<ZC_CO_Surface<ZC_Vec3<float>>> surfaces;
	static const uint triangle_size = 3;	//	e vertices in triangle
	if (SURFACES_IS_QUADS)		//	QUADS
	{
			//	In .dae stores triangles, we need quads, so take only half of triangles and add to them rest vertex to make a quad.
			//	The second part of the vertices stores triangles that complement the triangles from the first half of the vertices to the quad.
			//	From that triangles we need only second index of triangle, rest to vertices of quad we allready have (first and third indexes).
		const uint quad_size = 4;	//	4 vertices on quad
		const uint num_quad_faces = pMesh->mNumFaces / 2;		//	pMesh->mNumFaces have triangles count, we need quads
		const uint num_vertices = pMesh->mNumVertices / 2;	//	need only falf of vertices
			
			//	return true if surface was added to the surfaces, otherwise false
		auto lamb_add_cylinder_suf = [&surfaces, pMesh, num_quad_faces, lamb_CalculateVector](uint v_i, uint quad_i) -> bool
		{
			ZC_Vec3<float> normal = G_Assimp_ZC_Converter::GetVec3(pMesh->mNormals[v_i]);
			if (normal[2] == 1.f || normal[2] == -1.f)		//	in our cylinder if normal.z is 1 or -1 then x and y are always 0, but rounding with floating point It's too stupid to make it 0 in all cases. So sometimes we can get values ​​close to 0, but we know they should be 0.
			{
				normal[0] = 0.f;
				normal[1] = 0.f;
			}
			for (ZC_CO_Surface<ZC_Vec3<float>>& surf : surfaces)
			{
				if (surf.normal == normal)
				{
					for (uint triangle_i = 0; triangle_i < triangle_size; ++triangle_i)	//	add unadded points of the triangle of the quad
					{
						ZC_Vec3<float> point = lamb_CalculateVector(pMesh->mVertices[v_i + triangle_i]);
						if (!ZC_Find(surf.points, point))
						{
							surf.points.reserve(surf.points.size() + 1);
							surf.points.emplace_back(point);
						}
					}

					uint v4_i = (num_quad_faces + quad_i) * triangle_size + 1;		//	index of 4th vertex of quad, from second part of vertices
					ZC_Vec3<float> point = lamb_CalculateVector(pMesh->mVertices[v4_i]);
					if (!ZC_Find(surf.points, point)) surf.points.emplace_back(point);
					return true;
				}
			}
			return false;
		};

		surfaces.reserve(num_quad_faces);
		for (uint v_i = 0, quad_i = 0; v_i < num_vertices; v_i += triangle_size, ++quad_i)
		{
			if (model_name == G_MN__Platform_cylinder_black && lamb_add_cylinder_suf(v_i, quad_i)) continue;		//	this was the top or bottom surface added to an existing surface

			std::vector<ZC_Vec3<float>> quad_verts;
			quad_verts.reserve(quad_size);
			for (uint triangle_i = 0; triangle_i < triangle_size; ++triangle_i)	//	fill triangle of the quad
				quad_verts.emplace_back(lamb_CalculateVector(pMesh->mVertices[v_i + triangle_i]));

			uint v4_i = (num_quad_faces + quad_i) * triangle_size + 1;		//	index of 4th vertex of quad, from second part of vertices
			quad_verts.emplace_back(lamb_CalculateVector(pMesh->mVertices[v4_i]));

			surfaces.emplace_back(ZC_CO_Surface<ZC_Vec3<float>>(std::move(quad_verts), G_Assimp_ZC_Converter::GetVec3(pMesh->mNormals[v_i])));		//	surface have same normal in all vertices, take it no metter from wich vertex
		}
	}
	else	//	TRIANGLES
	{
		surfaces.reserve(pMesh->mNumFaces);
		for (uint v_i = 0; v_i < pMesh->mNumVertices; v_i += triangle_size)
		{
			std::vector<ZC_Vec3<float>> triangle_verts;
			triangle_verts.reserve(triangle_size);
			for (size_t triangle_i = 0; triangle_i < triangle_size; ++triangle_i)	//	fill triangle
				triangle_verts.emplace_back(lamb_CalculateVector(pMesh->mVertices[v_i + triangle_i]));

			surfaces.emplace_back(ZC_CO_Surface<ZC_Vec3<float>>(std::move(triangle_verts), G_Assimp_ZC_Converter::GetVec3(pMesh->mNormals[v_i])));		//	surface have same normal in all vertices, take it no metter from wich vertex
		}
	}
	return surfaces;
}

ZC_DrawerSet G_ModelLoader::CreateDrawerSet(aiNode* pNode, const aiScene* pScene, const std::string& path, bool smooth_normals, bool invert_normals, G_ModelName model_name)
{
		//	get model matrix if it is
	static const aiMatrix4x4 model_one;
	aiMatrix4x4* pModel = model_one != pNode->mTransformation ? &(pNode->mTransformation) : nullptr;
	ZC_Mat4<float> model = pModel ? G_Assimp_ZC_Converter::GetMat4(*pModel) : ZC_Mat4<float>(1.f);
	ZC_Mat4<float> model_scale_rotate = model;
	if (pModel) model_scale_rotate[3] = ZC_Vec4<float>(0.f, 0.f, 0.f, 1.f);

		//	count vertices
	uint verts_count = 0;
	for(uint i = 0; i < pNode->mNumMeshes; i++)
		verts_count += pScene->mMeshes[pNode->mMeshes[i]]->mNumVertices / 6 * 4;	//	need only 4 vertices equal corners of the quad for elements rendering, don't need repeat vertices in one quad. See lower at mesh reading.

	char gpu_object_id_for_lightning = 0;
	switch (model_name)
	{
	case G_MN__SphereMarble: gpu_object_id_for_lightning = 2; break;	//	will be convert to -1 in GL_INT_2_10_10_10_REV
	case G_MN__Platform_cylinder_black: gpu_object_id_for_lightning = 0; break;
	case G_MN__SphereMap: gpu_object_id_for_lightning = 1; break;
	default: break;
	}

		//	vbo
	std::list<std::list<VertNorm>> verts_to_smooth;
	std::vector<Vertex> vertices;
	vertices.reserve(verts_count);

	auto lamb_load_Vertex = [pModel, &model, invert_normals, smooth_normals, &model_scale_rotate, gpu_object_id_for_lightning, &vertices, &verts_to_smooth](aiMesh* pMesh, uint vert_i)
	{
		Vertex vertex;
		if (pModel)
		{
			ZC_Vec4<float> pos = pModel ? model * ZC_Vec4<float>(G_Assimp_ZC_Converter::GetVec3(pMesh->mVertices[vert_i]), 1.f) : ZC_Vec4<float>(G_Assimp_ZC_Converter::GetVec3(pMesh->mVertices[vert_i]), 1.f);
			vertex.position = { pos[0], pos[1], pos[2] };
		}
		else vertex.position = G_Assimp_ZC_Converter::GetVec3(pMesh->mVertices[vert_i]);
			//	normal
		aiVector3D& normal = pMesh->mNormals[vert_i];
		if (invert_normals) normal *= -1.f;		//	invert if need
		if (!smooth_normals)	//	set normal if don't need make smoth (later)
		{
			if (pModel)		//	need rotate and scale with pModel
			{
				ZC_Vec3<float> new_normal = ZC_Vec::Normalize(ZC_Vec::Vec4_to_Vec3(model_scale_rotate * ZC_Vec4<float>(normal.x, normal.y, normal.z, 1.f)));
				vertex.normal = ZC_Pack_INT_2_10_10_10_REV(new_normal[0], new_normal[1], new_normal[2], gpu_object_id_for_lightning);
			}
			else vertex.normal = ZC_Pack_INT_2_10_10_10_REV(normal.x, normal.y, normal.z, gpu_object_id_for_lightning);
		}

			//	tex coords
		if (pMesh->mTextureCoords[0])
		{
			aiVector3D& tex_coords = pMesh->mTextureCoords[0][vert_i];
				//	don't use pack, texture may have coords out of range [0,1]
			vertex.texCoords = ZC_Vec2<float>(tex_coords.x, tex_coords.y);
		}
		Vertex& v = vertices.emplace_back(vertex);
		
		if (smooth_normals)		//	add same vertices to lists to recalculate normals
		{
			bool new_vertex = true;
			for (auto& same_verts : verts_to_smooth)
			{
				if (same_verts.front().pVert->position == v.position)
				{
					same_verts.emplace_back(VertNorm{ .pVert = &v, .normal = {normal.x, normal.y, normal.z} });
					new_vertex = false;
					break;
				}
			}
			if (new_vertex) verts_to_smooth.emplace_back(std::list<VertNorm>{ VertNorm{ .pVert = &v, .normal = ZC_Vec::Normalize<float>({ normal.x, normal.y, normal.z }) } });
		}
	};

	for(uint mesh_i = 0; mesh_i < pNode->mNumMeshes; mesh_i++)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[mesh_i]];
		uint quads_count = pMesh->mNumVertices / 6;
			//	assimp store at first all first triangles of the quads of the mesh, then second triangles of the quads. The order of the corners of triangles: first - tl, br, bl; second - tl, tr, br.
			//	So, we need all the vertices of the first triangle and only the tr of the second.
		uint offset_to_second_triangles = pMesh->mNumVertices / 2;	//	index of tl of the first of the second triangles
		for (uint vert_i = 0; vert_i < offset_to_second_triangles; vert_i++)
		{
			lamb_load_Vertex(pMesh, vert_i);	//	read tl, br, bl from the first triangle
			if ((vert_i + 1) % 3 == 0) lamb_load_Vertex(pMesh, offset_to_second_triangles + vert_i - 1);	//	when tl, br, bl is read from the first triangle, add tr from second triangle
		}
	}
	if (smooth_normals)
	{
		for (auto& same_verts : verts_to_smooth)
		{
			ZC_Vec3<float> smooth_normal;
			for (VertNorm& v : same_verts) smooth_normal += v.normal;
			smooth_normal /= same_verts.size();
			int normal_int = 0;
			
			if (pModel)		//	need rotate and scale with pModel
			{
				ZC_Vec3<float> new_normal = ZC_Vec::Normalize(ZC_Vec::Vec4_to_Vec3(model_scale_rotate * ZC_Vec4<float>(smooth_normal, 1.f)));
				normal_int = ZC_Pack_INT_2_10_10_10_REV(new_normal[0], new_normal[1], new_normal[2], gpu_object_id_for_lightning);
			}
			else normal_int = ZC_Pack_INT_2_10_10_10_REV(smooth_normal[0], smooth_normal[1], smooth_normal[2], gpu_object_id_for_lightning);
			for (VertNorm& v :same_verts) v.pVert->normal = normal_int;		//change normal for vertices
		}
	}
	ZC_Buffer vbo(GL_ARRAY_BUFFER);
	vbo.GLNamedBufferStorage(vertices.size() * sizeof(Vertex), vertices.data(), 0u);
	
	GLenum elementsType = 0;
	ul_zc elements_count = 0;
		//	draw order triangles of the quad: first(tl, br, bl), second(tl, tr, bl). GetTriangleElements() in description have another corners order (bl, tr, tl	bl, br, tr). But in both cases we will get order of indexes for first quad (0, 1, 2,  0, 3, 2) and so on for other quads...
	ZC_DA<uchar> elements = ZC_Buffer::GetTriangleElements(elements_count, elementsType, vertices.size() / 4, 0);
	
	ZC_Buffer ebo(GL_ELEMENT_ARRAY_BUFFER);
	ebo.GLNamedBufferStorage(elements.size, elements.Begin(), 0u);
		
		//	glDraw
	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, elements_count, elementsType, 0u);

		//	ShPInitSet
	typename ZC_ShProgs::ShPInitSet* pShPIS =
		model_name == G_MN__SphereMarble || model_name == G_MN__SphereMap || model_name == G_MN__Platform_cylinder_black ? ZC_ShProgs::Get(ShPN_Game_Sphere) 
		: model_name == G_MN__SphereStar ? ZC_ShProgs::Get(ShPN_Game_Star)
		: nullptr;

		//	vao
	ZC_VAO vao;
	vao.Config(pShPIS->vaoConfigData, vbo, &ebo, 0, 0);

	std::forward_list<ZC_Buffer> buffers;
	buffers.emplace_front(std::move(vbo));
	buffers.emplace_front(std::move(ebo));

		//	texture
	std::forward_list<ZC_TexturesSet> tex_sets;
	aiMaterial* material = pScene->mMaterials[pScene->mMeshes[pNode->mMeshes[0]]->mMaterialIndex];	//	each object have only one texture in my case, so no need to make cycle by meshes as upper for vertices
	// aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
	{
			// retrieve the directory path of the filepath
		std::string directory = path.substr(0, path.find_last_of('/'));
		aiString str;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string tex_path = directory + "/" + std::string(str.C_Str());

		ZC_TexSets::VectorOfTexturesCreator texCreator = pShPIS->texSets.GetCreator();
		auto pNext_tex_name = texCreator.NextName();
		if (pNext_tex_name && *pNext_tex_name == ZC_TexSets::texColor)
			texCreator.Add(ZC_Texture::LoadTexture2D(tex_path.c_str(), 0, false, GL_REPEAT, GL_REPEAT));
		
		tex_sets.emplace_front(ZC_TexturesSet{ .id = 0, .textures = texCreator.GetVector() });
	}

	// vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	// textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	// textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	// textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	// textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		
	return ZC_DrawerSet(pShPIS, std::move(vao), std::move(upDraw), std::move(buffers), std::move(tex_sets));
}
#endif