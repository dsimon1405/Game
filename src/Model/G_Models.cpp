#include "G_Models.h"

#include <ZC/Tools/Container/ZC_ContFunc.h>
#include "G_ModelLoader.h"
#include "G_CubeModelCreator.h"

#include <ZC/Tools/Time/ZC_Timer.h>
void G_Models::LoadModels()
{
#ifdef G_NewV
	models.reserve(2);
	models.emplace_back(G_ModelLoader::LoadModel(G_MN__SphereMarble));
	models.emplace_back(G_ModelLoader::LoadModel(G_MN__Platform_cylinder_black));
#else
	models.reserve(G_MN__Platform_cube);
    ZC_Timer t1(ZC_TR__repeats, 1.0, ZC_TRO__seconds_total, "total load");
    t1.StartPoint();
	for (int i = 0.f; i < G_MN__Platform_cube; ++i)
	{
		auto& iter = models.emplace_back(G_ModelLoader::LoadModel((G_ModelName)i));
		if ((G_ModelName)i != G_MN__SphereMap && (G_ModelName)i != G_MN__SphereStar) iter.radius = ZC_Vec::Length(GetFarestVertexOfSurface(iter.surfaces));	//	set radius
	}
	t1.EndPoint();
	// models.emplace_back(G_CubeModelCreator::LoadModel());	//	load quad platform, not from blender, hard coded vertices
#endif
}

ZC_DSController G_Models::GetModel_DSController(G_ModelName model_name, int texSetId, std::forward_list<ZC_uptr<ZC_RSPersonalData>>&& personalData)
{
	return ZC_Find(models, model_name)->drawer_set.MakeZC_DSController(texSetId, std::move(personalData));
}

ZC_uptr<ZC_CO_FigureSphere> G_Models::GetModel_COFigure(G_ModelName model_name, const ZC_Vec3<float>& scale)
{
 	G_ModelSet* pMS = ZC_Find(models, model_name);
	return new ZC_CO_Figure({ 0.f, 0.f, 0.f }, scale == ZC_Vec3<float>(1.f, 1.f, 1.f) ? pMS->radius : CalculateScaledRadius(GetFarestVertexOfSurface(pMS->surfaces), scale),
		pMS->surfaces);
}

float G_Models::CalculateRadius(G_ModelName model_name, const ZC_Vec3<float>& scale)
{
	return CalculateScaledRadius(GetFarestVertexOfSurface(ZC_Find(models, model_name)->surfaces), scale);
}

float G_Models::GetRadius(G_ModelName model_name)
{
	return ZC_Find(models, model_name)->radius;
}

const ZC_Vec3<float>& G_Models::GetFarestVertexOfSurface(const std::vector<ZC_CO_Surface<ZC_Vec3<float>>>& _surfaces)
{
	LengthKeeper lengthes[]
	{
		LengthKeeper{ .v = &(_surfaces[0].points[0]), .length = ZC_Vec::Length(_surfaces[0].points[0]) },
		LengthKeeper{ .v = &(_surfaces[0].points[1]), .length = ZC_Vec::Length(_surfaces[0].points[1]) },
		LengthKeeper{ .v = &(_surfaces[0].points[2]), .length = ZC_Vec::Length(_surfaces[0].points[2]) },
		LengthKeeper{ .v = &(_surfaces[0].points[3]), .length = ZC_Vec::Length(_surfaces[0].points[3]) }
	};
	std::sort(lengthes, lengthes + 3, [](LengthKeeper& lk1, LengthKeeper& lk2) { return lk1.length > lk2.length; });
	return *(lengthes[0].v);
}

float G_Models::CalculateScaledRadius(const ZC_Vec3<float>& _farest_vertex, const ZC_Vec3<float>& scale)
{
	ZC_Vec4<float> pos = ZC_Mat4<float>(1.f).Scale(scale) * ZC_Vec4<float>(_farest_vertex, 1.f);
	return ZC_Vec::Length<float>({ pos[0], pos[1], pos[2] });
}