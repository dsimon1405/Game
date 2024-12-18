#pragma once

#include "G_ModelSet.h"
#include <ZC/Tools/Math/ZC_Figures.h>
#include <ZC/Collision/ZC_CO_Figure.h>

#include <vector>

struct G_CubeModelCreator
{
    static G_ModelSet LoadModel();

private:
    static ZC_DA<uchar> GetTriangleElements(ul_zc& rElementsCount, GLenum& rElementsType, ul_zc quad_size);

    static std::vector<ZC_Quad> GetCoords();
    static std::vector<int> GetNormals();
    static std::vector<ushort> GetTexCoords();
        //  scale - dont scale model, just calculate radius. Scaling done by caller in model matrix
    static std::vector<ZC_CO_Surface<ZC_Vec3<float>>> Get_COSurfaces();
    static float GetPlatformRadius(const ZC_Mat4<float>& model);

    template<typename T>
    static void FillTriangleElements(T* pElementsHead, ul_zc elementsSize, ul_zc quadsElementsCount)
    {
        ulong elementsI = 0,
            index = 0;
        for ( ; elementsI < quadsElementsCount; elementsI += 6)
        {
            pElementsHead[elementsI] = pElementsHead[elementsI + 3] = static_cast<T>(index++);
            pElementsHead[elementsI + 1] = pElementsHead[elementsI + 5] = static_cast<T>(index++);
            pElementsHead[elementsI + 2] = static_cast<T>(index++);
            pElementsHead[elementsI + 4] = static_cast<T>(index++);
        }
        while (elementsI < elementsSize)
        {
            pElementsHead[elementsI++] = static_cast<T>(index++);
            pElementsHead[elementsI++] = static_cast<T>(index++);
            pElementsHead[elementsI++] = static_cast<T>(index++);
        }
    }
};