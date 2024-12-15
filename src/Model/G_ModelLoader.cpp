#include "G_ModelLoader.h"

#include <ZC/File/ZC_File.h>
#include "G_Assimp_ZC_Converter.h"

#include <assimp/postprocess.h>

#include <list>

G_ModelSet G_ModelLoader::LoadModel(G_ModelName model_name)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	std::string model_path = GetPath(model_name);
	
	const aiScene* pScene = importer.ReadFile(model_path, 0);
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
	if (model_name != G_MN__SphereMap && model_name != G_MN__SphereStar) co_surfs = LoadCollisionSurfaces(pNode->mChildren[collision_index], pScene);	
	
	return G_ModelSet{ .model_name = model_name, .drawer_set = CreateDrawerSet(pNode->mChildren[draw_index], pScene, path,
		(model_name == G_MN__SphereMarble || model_name == G_MN__SphereMap), model_name == G_MN__SphereMap, model_name),
		.surfaces = std::move(co_surfs) } ;

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

std::vector<ZC_CO_Surface<ZC_Vec3<float>>> G_ModelLoader::LoadCollisionSurfaces(aiNode *pNode, const aiScene *pScene)
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
	const uint triangle_size = 3;	//	e vertices in triangle
	if (SURFACES_IS_QUADS)		//	QUADS
	{
			//	In .dae stores triangles, we need quads, so take only half of triangles and add to them rest vertex to make a quad.
			//	The second part of the vertices stores triangles that complement the triangles from the first half of the vertices to the quad.
			//	From that triangles we need only second index of triangle, rest to vertices of quad we allready have (first and third indexes).
		const uint quad_size = 4;	//	4 vertices on quad
		const uint num_quad_faces = pMesh->mNumFaces / 2;		//	pMesh->mNumFaces have triangles count, we need quads
		const uint num_vertices = pMesh->mNumVertices / 2;	//	need only falf of vertices
		surfaces.reserve(num_quad_faces);
		for (uint v_i = 0, quad_i = 0; v_i < num_vertices; v_i += triangle_size, ++quad_i)
		{
			std::vector<ZC_Vec3<float>> quad_verts;
			quad_verts.reserve(quad_size);
			for (uint triangle_i = 0; triangle_i < triangle_size; ++triangle_i)	//	fill triangle
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
		verts_count += pScene->mMeshes[pNode->mMeshes[i]]->mNumVertices;

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
	for(uint mesh_i = 0; mesh_i < pNode->mNumMeshes; mesh_i++)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[mesh_i]];
		for (uint vert_i = 0; vert_i < pMesh->mNumVertices; vert_i++)
		{
			Vertex vertex;
			if (pModel)
			{
				ZC_Vec4<float> pos = model * ZC_Vec4<float>(G_Assimp_ZC_Converter::GetVec3(pMesh->mVertices[vert_i]), 1.f);
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

		//	ebo
	// 		//	elemnts count
	// ulong elementsCount = 0;
	// for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	// {
	// 	aiFace& face = pMesh->mFaces[i];
	// 	for (unsigned int j = 0; j < face.mNumIndices; j++) ++elementsCount;
	// }
			//	get elements type and size, and fill
	GLenum elementsType = 0;
	ulong storingTypeSize = 0;
	ZC_Buffer::GetElementsData(vertices.size() - 1ull, storingTypeSize, elementsType);
	// ZC_DA<uchar> elements(storingTypeSize * elementsCount);
	ZC_DA<uchar> elements(storingTypeSize * verts_count);	//	from blender elements count equal verts count...
	switch (storingTypeSize)
	{
	case 1: FillElements<uchar>(elements.pHead, verts_count); break;
	case 2: FillElements<ushort>(reinterpret_cast<ushort*>(elements.pHead), verts_count); break;
	case 4: FillElements<uint>(reinterpret_cast<uint*>(elements.pHead), verts_count); break;
	}
	// switch (storingTypeSize)
	// {
	// case 1: FillElements<uchar>(elements.pHead, pMesh); break;
	// case 2: FillElements<ushort>(reinterpret_cast<ushort*>(elements.pHead), pMesh); break;
	// case 4: FillElements<uint>(reinterpret_cast<uint*>(elements.pHead), pMesh); break;
	// }
	ZC_Buffer ebo(GL_ELEMENT_ARRAY_BUFFER);
	ebo.GLNamedBufferStorage(elements.size, elements.Begin(), 0u);
		
		//	glDraw
	// ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, elementsCount, elementsType, 0);
	ZC_uptr<ZC_GLDraw> upDraw = ZC_uptrMakeFromChild<ZC_GLDraw, ZC_DrawElements>(GL_TRIANGLES, verts_count, elementsType, 0u);

		//	ShPInitSet
	typename ZC_ShProgs::ShPInitSet* pShPIS =
		model_name == G_MN__SphereMarble || model_name == G_MN__SphereMap || model_name == G_MN__Platform_cylinder_black ? ZC_ShProgs::Get(ShPN_Game_Sphere) 
		: model_name == G_MN__SphereStar ? ZC_ShProgs::Get(ShPN_Game_Star)
		: nullptr;

	// switch (model_name)
	// {
	// case : break;
	
	// default:
	// 	break;
	// }

		//	vao
	ZC_VAO vao;
	vao.Config(pShPIS->vaoConfigData, vbo, &ebo, 0, 0);   //  vertices count in vbo (in one quad 4, in one triangle 3)

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