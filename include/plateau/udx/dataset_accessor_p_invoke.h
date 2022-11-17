#pragma once

#include <plateau/udx/i_dataset_accessor.h>

namespace plateau::udx{
    /**
     * \brief IDatasetAccessor を P/Invoke で扱うためのクラスです。
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
        const GmlFileInfo* resultOfGetGmlFiles(int index) const;
        int resultOfGetGmlFilesCount() const;

    private:
        std::shared_ptr<IDatasetAccessor> accessor_;
        std::vector<GmlFileInfo> result_of_get_gml_files_;
    };
}
