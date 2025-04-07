#include "plateau/dataset/standard_map_grid.h"
#include "plateau/geometry/geo_coordinate.h"
#include <stdexcept>

namespace plateau::dataset {

    StandardMapGrid::StandardMapGrid(const std::string& code) : code_(code), is_valid_(false) {
        // コードの形式を検証
        // TODO
        is_valid_ = true;
    }

    std::string StandardMapGrid::get() const {
        return code_;
    }

    geometry::Extent StandardMapGrid::getExtent() const {
        if (!isValid()) {
            throw std::runtime_error("Invalid standard map grid code.");
        }
        
        // TODO: 図郭コードから緯度経度範囲を計算する実装を追加
        // この実装は図郭コードの仕様に基づいて行う必要があります
        return geometry::Extent(geometry::GeoCoordinate(0, 0, 0), geometry::GeoCoordinate(0, 0, 0));
    }

    bool StandardMapGrid::isWithin(const GridCode& other) const {
        if (!isValid()) return false;
        
        // 同じ型の場合のみ比較
        const auto* other_grid = dynamic_cast<const StandardMapGrid*>(&other);
        if (other_grid == nullptr) return false;
        
        // TODO: 図郭の包含関係を判定する実装を追加
        return false;
    }

    bool StandardMapGrid::isValid() const {
        return is_valid_;
    }

    bool StandardMapGrid::operator==(const StandardMapGrid& other) const {
        return code_ == other.code_;
    }

    bool StandardMapGrid::operator<(StandardMapGrid& other) const {
        return code_ < other.code_;
    }

    bool StandardMapGrid::operator<(const StandardMapGrid& other) const {
        return code_ < other.code_;
    }
} 