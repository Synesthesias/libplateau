#pragma once

#include <plateau/io/mesh_convert_options.h>
#include <citygml/vecs.hpp>

namespace plateau::geometry {
// 機能的には MeshConvertOptions と共通部分が多いですが、
// 仕様変更にともなって MeshConvertOptions は利用されなくなる見込みなのでここに新しく作り直します。
    struct MeshExtractOptions {
        MeshExtractOptions(TVec3d referencePoint, AxesConversion meshAxes, MeshGranularity meshGranularity, unsigned maxLod, unsigned minLod, bool exportAppearance, int gridCountOfSide):
                referencePoint(referencePoint),
                meshAxes(meshAxes),
                meshGranularity(meshGranularity),
                maxLOD(maxLod),
                minLOD(minLod),
                exportAppearance(exportAppearance),
                gridCountOfSide(gridCountOfSide){}

    public:
        TVec3d referencePoint;
        AxesConversion meshAxes;
        MeshGranularity meshGranularity;
        int maxLOD;
        int minLOD;
        bool exportAppearance;
        /**
         * グリッド分けする時の、1辺の分割数です。
         * この数の2乗がグリッドの数になります。
         */
        int gridCountOfSide;
    };
}