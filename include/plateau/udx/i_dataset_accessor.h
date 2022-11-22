#pragma once

#include <libplateau_api.h>
#include <plateau/udx/gml_file.h>
#include <plateau/udx/city_model_package.h>
//#include <plateau/udx/mesh_code.h>
//#include <plateau/geometry/geo_coordinate.h>
//#include "plateau/geometry/geo_reference.h"
#include <set>

namespace plateau::udx {
    class LIBPLATEAU_EXPORT IDatasetAccessor {
    public:
        // TODO
        virtual std::vector<GmlFile> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) = 0;
        /// 上記メソッドのP/Invoke版です。
        virtual void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package, std::vector<GmlFile>& out_vector) = 0;

        virtual std::vector<MeshCode> getMeshCodes() = 0;
        /// 上記メソッドのP/Invoke版です。
        virtual void getMeshCodes(std::vector<MeshCode>& mesh_code) = 0;

        /// 含まれるパッケージ種をフラグで返します。
        virtual PredefinedCityModelPackage getPackages() = 0;

//        virtual DatasetMetadata getMetadata(MeshCode mesh_code) = 0;
        virtual int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) = 0;
        virtual ~IDatasetAccessor() = default;
    };
}
