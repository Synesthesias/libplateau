#pragma once

#include <plateau/udx/i_dataset_accessor.h>

namespace plateau::udx{
    /**
     * \brief IDatasetAccessor を P/Invoke で扱うためのラッパークラスです。
     * vector を返す関数などはそのままだと P/Invoke で扱えないため、
     * 関数の戻り値を保持しておいて、インデックスで1つづつ P/Invoke でアクセスできるようにします。
     */
    class LIBPLATEAU_EXPORT DatasetAccessorPInvoke{
    public:

        /**
         * ここで渡した accessor を P/Invoke の対象とします。
         */
        explicit DatasetAccessorPInvoke(std::shared_ptr<IDatasetAccessor> accessor);

        void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package);
        const GmlFile* resultOfGetGmlFiles(PredefinedCityModelPackage package, int index) const;
        int resultOfGetGmlFilesCount(PredefinedCityModelPackage package) const;

        void getMeshCodes();
        MeshCode resultOfGetMeshCodes(int index) const;
        int resultOfGetMeshCodesCount() const;

        int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package);

    private:
        std::shared_ptr<IDatasetAccessor> accessor_;
        std::map<PredefinedCityModelPackage, std::vector<GmlFile*>> result_of_get_gml_files_;
        std::vector<MeshCode> result_of_get_mesh_codes_;
    };
}
