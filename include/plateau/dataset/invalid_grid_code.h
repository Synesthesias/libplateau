#pragma once

#include <libplateau_api.h>
#include "plateau/dataset/grid_code.h"
#include "plateau/geometry/geo_coordinate.h"

namespace plateau::dataset {
    /**
     * \brief 無効なグリッドコードを表すクラスです。
     * 
     * GridCodeの派生クラスとして、無効なグリッドコードを表現します。
     * このクラスのインスタンスは常に無効（isValid() == false）です。
     */
    class LIBPLATEAU_EXPORT InvalidGridCode : public GridCode {
    public:
        InvalidGridCode() = default;

        /**
         * \brief 無効なグリッドコードを文字列として取得します。
         * \return 常に空文字列を返します。
         */
        std::string get() const override { return ""; }

        /**
         * \brief 無効なグリッドコードの緯度経度範囲を取得します。
         * \return 原点（0,0,0）を中心とする無効な範囲を返します。
         */
        geometry::Extent getExtent() const override {
            return geometry::Extent(
                geometry::GeoCoordinate(0, 0, 0),
                geometry::GeoCoordinate(0, 0, 0)
            );
        }

        /**
         * \brief このコードが他のコードに内包されるかどうかを計算します。
         * \return 常にfalseを返します。
         */
        bool isWithin(const GridCode& other) const override { return false; }

        /**
         * \brief コードが適切な値かどうかを返します。
         * \return 常にfalseを返します。
         */
        bool isValid() const override { return false; }

        /**
         * \brief １段階上のレベルのグリッドコードに変換します。
         * \return 無効なグリッドコードを返します。
         */
        std::shared_ptr<GridCode> upper() override { 
            return std::make_shared<InvalidGridCode>(); 
        }

        int getLevel() const override { return -1; }
        bool isLargestLevel() const override { return true; }
    };
} 