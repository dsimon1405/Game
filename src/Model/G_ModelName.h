#pragma once

#include <System/G_NewV.h>
#ifdef G_NewV
enum G_ModelName
{
	G_MN__SphereMarble,
	G_MN__Platform_cylinder_black,
};
#else
enum G_ModelName
{
	G_MN__SphereMarble,
	G_MN__SphereStar,
	G_MN__SphereMap,
	G_MN__Platform_cylinder_black,

	G_MN__Platform_cube,
};
#endif