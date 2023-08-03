#pragma once

#include <plateau/polygon_mesh/model.h>
#include <plateau/geometry/geo_reference.h>

namespace plateau::polygonMesh {
    using namespace plateau::geometry;
    /**
     * モデルに航空写真または地図を貼り付けます。
     */
    class MapAttacher {
    public:
        /**
         * 引数のModelに含まれる各Meshに対し、航空写真または地図を貼り付けます。
         */
        static void attach(Model& model, const GeoReference& geo_reference);
    };

}
