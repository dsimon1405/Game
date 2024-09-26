#pragma once

#include <ZC/Tools/Math/ZC_Mat4.h>

#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

namespace G_Assimp_ZC_Converter
{
	ZC_Vec3<float> GetVec3(const aiVector3D& vec);
	ZC_Mat4<float> GetMat4(const aiMatrix4x4& mat);
};