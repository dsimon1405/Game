#pragma once

#include "G_ModelSet.h"
#include <ZC/Collision/ZC_CO_Figure.h>

struct G_Models
{
	static void LoadModels();
	static ZC_DSController GetModel_DSController(G_ModelName model_name, int texSetId = -1, std::forward_list<ZC_uptr<ZC_RSPersonalData>>&& personalData = {});
        //  scale - dont scale model, just calculate radius. Scaling done by caller in model matrix
	static ZC_CO_Figure GetModel_COFigure(G_ModelName model_name, const ZC_Vec3<float>& scale = {1.f, 1.f, 1.f});

private:
	struct LengtheKeeper
	{
		const ZC_Vec3<float>* v;
		float length;
	};

	static inline std::vector<G_ModelSet> models;
};