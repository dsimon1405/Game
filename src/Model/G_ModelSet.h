#pragma once

#include <ZC/Video/OpenGL/Renderer/ZC_DrawerSet.h>
#include <ZC/Collision/ZC_CO_Surface.h>

enum G_ModelName
{
	G_MN__Sphere,
	G_MN__Platform_cylinder_black,

	G_MN__Platform_cube,

	G_MN__sizeof
};

struct G_ModelSet
{
	G_ModelName model_name;
	ZC_DrawerSet drawer_set;
	std::vector<ZC_CO_Surface<ZC_Vec3<float>>> surfaces;
	float radius = 0.f;		//	sets in G_Models::GetModel_COFigure()

	bool operator == (G_ModelName _model_name);
};