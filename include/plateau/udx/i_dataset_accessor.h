#pragma once

#include <libplateau_api.h>
#include <plateau/udx/gml_file_info.h>
#include <plateau/udx/city_model_package.h>
//#include <plateau/udx/mesh_code.h>
//#include <plateau/geometry/geo_coordinate.h>
//#include "plateau/geometry/geo_reference.h"
#include <set>

namespace plateau::udx {
    class LIBPLATEAU_EXPORT IDatasetAccessor {
    public:
        // TODO
//        virtual std::vector<GmlFile> findGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) = 0;
        virtual std::set<MeshCode>& getMeshCodes() = 0;
//        virtual DatasetMetadata getMetadata(MeshCode mesh_code) = 0;
//        virtual int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) = 0;
    };
}
