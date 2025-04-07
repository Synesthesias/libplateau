#pragma once

#include <string>

#include <libplateau_api.h>
#include "plateau/geometry/geo_coordinate.h"
#include "plateau/dataset/grid_code.h"

namespace plateau::dataset {
    /**
     * \brief 国土基本図図郭を表します。
     * 
     * 国土基本図の図郭コードを扱い、緯度経度範囲の取得などの機能を提供します。
     */
    class LIBPLATEAU_EXPORT StandardMapGrid : public GridCode {
    public:
        explicit StandardMapGrid(const std::string& code);
        StandardMapGrid() = default;

        /**
         * \brief 図郭コードを文字列として取得します。
         */
        std::string get() const override;

        /**
         * \brief 図郭の緯度経度範囲を取得します。
         */
        geometry::Extent getExtent() const override;

        /**
         * \brief 図郭が他の図郭に内包されるかどうかを計算します。
         */
        bool isWithin(const GridCode& other) const override;

        /**
         * \brief 図郭コードが適切な値かどうかを返します。
         */
        bool isValid() const override;

        bool operator==(const StandardMapGrid& other) const;
        bool operator<(StandardMapGrid& other) const;
        bool operator<(const StandardMapGrid& other) const;

    private:
        std::string code_;  // 図郭コード
        bool is_valid_;     // コードが有効かどうか
    };
} 