#include "G_Assimp_ZC_Converter.h"
#ifndef G_NewV

namespace G_Assimp_ZC_Converter
{
	ZC_Vec3<float> GetVec3(const aiVector3D& vec) 
	{ 
		return ZC_Vec3<float>(vec.x, vec.y, vec.z); 
	}

	ZC_Mat4<float> GetMat4(const aiMatrix4x4& mat)
	{
		return ZC_Mat4<float>
		(
			{ mat.a1, mat.b1, mat.c1, mat.d1 },
			{ mat.a2, mat.b2, mat.c2, mat.d2 },
			{ mat.a3, mat.b3, mat.c3, mat.d3 },
			{ mat.a4, mat.b4, mat.c4, mat.d4 }
		);
	}
};

#endif
