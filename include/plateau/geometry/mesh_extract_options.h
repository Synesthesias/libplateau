#pragma once

#include <plateau/io/mesh_convert_options.h>
#include <citygml/vecs.hpp>

namespace plateau::geometry {

    struct MeshExtractOptions {
        MeshExtractOptions(TVec3d referencePoint, AxesConversion meshAxes, MeshGranularity meshGranularity, unsigned maxLod, unsigned minLod, bool exportAppearance, int gridCountOfSide, float unitScale):
                referencePoint(referencePoint),
                meshAxes(meshAxes),
                meshGranularity(meshGranularity),
                maxLOD(maxLod),
                minLOD(minLod),
                exportAppearance(exportAppearance),
                gridCountOfSide(gridCountOfSide),
                unitScale(unitScale){}

    public:
        TVec3d referencePoint;
        AxesConversion meshAxes;
        MeshGranularity meshGranularity;
        unsigned maxLOD;
        unsigned minLOD;
        bool exportAppearance;
        /**
         * グリッド分けする時の、1辺の分割数です。
         * この数の2乗がグリッドの数となり、実際にはそれより細かくグループ分けされます。
         */
        int gridCountOfSide;
        float unitScale;
    };
}
