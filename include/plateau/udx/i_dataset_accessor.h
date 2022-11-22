#pragma once

#include <libplateau_api.h>
#include <plateau/udx/gml_file.h>
#include <plateau/udx/city_model_package.h>
#include <set>

namespace plateau::udx {
    class LIBPLATEAU_EXPORT IDatasetAccessor {
    public:
        /**
         * \brief GMLファイル群のうち、範囲が extent の内部であり、パッケージ種が package であるものを vector で返します。
         */
        virtual std::vector<GmlFile> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) = 0;
        /// 上記メソッドのP/Invoke版です。
        virtual void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                                 std::vector<GmlFile>& out_vector) = 0;

        /**
         * \brief 利用可能な MeshCode を検索して vector で返します。
         */
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
