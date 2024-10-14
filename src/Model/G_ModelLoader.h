#pragma once

#include "G_ModelSet.h"
#include <ZC/Tools/Math/ZC_Vec3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// #include <string>

class G_ModelLoader 
{
public:
    static G_ModelSet LoadModel(G_ModelName model_name);
    
private:
	static inline bool SURFACES_IS_QUADS = true;	//	TYPE OF SURFACES IN MODEL: QUADS OR TRIANGLES

	struct Vertex
	{
		// position
		ZC_Vec3<float> position;
		// normal
		int normal;
		// texCoords
		ZC_Vec2<ushort> texCoords;
	};
	struct VertNorm
	{
		G_ModelLoader::Vertex* pVert;
		ZC_Vec3<float> normal;
	};

	static std::string GetPath(G_ModelName model_name);

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    static G_ModelSet ProcessNode(G_ModelName model_name, aiNode* pNode, const aiScene* pScene, const std::string& path);
	static std::vector<ZC_CO_Surface<ZC_Vec3<float>>> LoadCollisionSurfaces(aiNode* pNode, const aiScene* pScene);
	static ZC_DrawerSet CreateDrawerSet(aiNode* pNode, const aiScene* pScene, const std::string& path, bool smooth_normals, bool invert_normals, G_ModelName model_name);

	// template <typename T>
	// static void FillElements(T* pElementsHead, aiMesh* pMesh)
	// {
	// 	for (uint i = 0, element_index = 0; i < pMesh->mNumFaces; i++)
	// 	{
	// 		aiFace& face = pMesh->mFaces[i];
	// 		for (uint j = 0; j < face.mNumIndices; j++)
	// 		{
	// 			pElementsHead[element_index] = (T)(face.mIndices[j]);
	// 			++element_index;
	// 		}
	// 	}
	// }
	template <typename T>
	static void FillElements(T* pElementsHead, size_t elements_count)
	{
		for (T i = 0; i < elements_count; ++i)
			pElementsHead[i] = T(i);
	}
};