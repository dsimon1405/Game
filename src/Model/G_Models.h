#pragma once

#include "G_ModelSet.h"
#include <ZC/Collision/ZC_CO_Figure.h>

struct G_Models
{
	static void LoadModels();
	static ZC_DSController GetModel_DSController(G_ModelName model_name, int texSetId = -1, std::forward_list<ZC_uptr<ZC_RSPersonalData>>&& personalData = {});
        //  scale - dont scale model, just calculate radius. Scaling done by caller in model matrix
	static ZC_CO_Figure GetModel_COFigure(G_ModelName model_name, const ZC_Vec3<float>& scale = {1.f, 1.f, 1.f});
	static float CalculateRadius(G_ModelName model_name, const ZC_Vec3<float>& scale);
		//	return correct radius
	static float GetRadius(G_ModelName model_name);

private:
	struct LengtheKeeper
	{
		const ZC_Vec3<float>* v;
		float length;
	};

	static inline std::vector<G_ModelSet> models;
		//	correct for model with origin at the {0,0,0}, and each of surfaces must have farest vertex (right geometry figures...)
	static const ZC_Vec3<float>& GetFarestVertexOfSurface(const std::vector<ZC_CO_Surface<ZC_Vec3<float>>>& _surfaces);
	static float CalculateScaledRadius(const ZC_Vec3<float>& _farest_vertex, const ZC_Vec3<float>& scale);
};