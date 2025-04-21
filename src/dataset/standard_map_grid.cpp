#include "plateau/dataset/standard_map_grid.h"
#include <algorithm>
#include "plateau/geometry/geo_coordinate.h"
#include <stdexcept>
#include <utility>
#include <plateau/geometry/geo_reference.h>
#include <plateau/polygon_mesh/mesh_extract_options.h>
#include <iostream>

namespace plateau::dataset {

    enum class StandardMapGridLevel
    {
        Invalid = -1,
        Level50000 = 0,
        Level5000 = 1,
        Level2500 = 2,
        Level1000 = 3,
        Level500 = 4,
    };

    namespace {
        constexpr int level5000_division_count = 10;
        constexpr int level2500_division_count = 2;
        constexpr int level1000_division_count = 5;
        constexpr int level500_division_count = 10;

        // 1セルのサイズ
        constexpr double level50000_cell_column = 30000.0;  // 南北30km
        constexpr double level50000_cell_row = 40000.0;  // 東西40km

        constexpr double level5000_cell_column = level50000_cell_column / level5000_division_count;   // 南北4km
        constexpr double level5000_cell_row = level50000_cell_row / level5000_division_count;   // 東西3km

        constexpr double level2500_cell_column = level5000_cell_column / level2500_division_count;    // 南北2km
        constexpr double level2500_cell_row = level5000_cell_row / level2500_division_count;    // 東西1.5km

        constexpr double level1000_cell_column = level5000_cell_column / level1000_division_count;    // 南北800m
        constexpr double level1000_cell_row = level5000_cell_row / level1000_division_count;    // 東西600m

        constexpr double level500_cell_column = level5000_cell_column / level500_division_count;      // 南北400m
        constexpr double level500_cell_row = level5000_cell_row / level500_division_count;      // 東西300m

        /**
         * 図郭コードの文字列からレベル（詳細度）を返します。
         */
        StandardMapGridLevel parseLevel(const std::string& code) {
            if (code.size() == 4) {
                return StandardMapGridLevel::Level50000;
            }
            if (code.size() == 6) {
                return StandardMapGridLevel::Level5000;
            }
            if (code.size() == 7) {
                return StandardMapGridLevel::Level2500;
            }
            if (code.size() == 8) {
                // 末尾がアルファベットであれば1000
                if (std::isalpha(code.back())) {
                    return StandardMapGridLevel::Level1000;
                }
                // 末尾が数字であれば500
                else {
                    return StandardMapGridLevel::Level500;
                }
            }

            // サポート対象外
            return StandardMapGridLevel::Invalid;
        }

        /**
         * 計算された平面直角座標をTVec3dのペアに変換します。
         */
        std::pair<TVec3d, TVec3d> createExtentPair(
            double min_column, double min_row,
            double max_column, double max_row) {
            // column: 南北方向（緯度）:Y軸
            // row: 東西方向（経度）：X軸
            return {TVec3d(min_row, 0, min_column), TVec3d(max_row, 0, max_column)};
        }

        void calculateSubGridExtent(
            double& min_column, double& min_row,
            double& max_column, double& max_row,
            int column_index, int row_index,
            double cell_column, double cell_row) {

            // column座標（南北）の計算
            min_column = min_column + (column_index * cell_column);
            max_column = min_column + cell_column;

            // row座標（東西）の計算
            min_row = min_row + (row_index * cell_row);
            max_row = min_row + cell_row;
        }
    }

    StandardMapGrid::StandardMapGrid(std::string code, bool is_valid = true) :
        code_(std::move(code)), is_valid_(is_valid) {
        try {
            // 図郭コードの文字列が数字とアルファベットからなることをチェックします。
            if (!std::all_of(code_.begin(), code_.end(), [](char c)
            {
                return std::isalnum(c);
            })) {
                is_valid_ = false;
                return;
            }

            // 図郭コードのレベル（詳細度）をチェックします。
            level_ = parseLevel(code_);
            if (level_ == StandardMapGridLevel::Invalid) {
                is_valid_ = false;
                return;
            }

            // 原点設定
            coordinate_origin_ = std::stoi(code_.substr(0, 2));

            // 図郭コードのcolumn, row座標を取得します。
            first_column_ = code_[2];  // 1文字目はcolumn座標（南北）
            first_row_ = code_[3];     // 2文字目はrow座標（東西）

            // 文字の範囲チェック
            if (first_column_ < 'A' || first_column_ > 'T' ||
                first_row_ < 'A' || first_row_ > 'H') {
                is_valid_ = false;
                return;
            }

            if (level_ == StandardMapGridLevel::Level50000) {
                return;
            }

            // 基点は左下（90）
            second_column_ = 9 - std::stoi(code_.substr(4, 1));
            second_row_ = std::stoi(code_.substr(5, 1));

            if (level_ == StandardMapGridLevel::Level5000) {
                return;
            }

            if (level_ == StandardMapGridLevel::Level2500) {
                // 基点は左下
                int third = std::stoi(code_.substr(6, 1));  // 1-4
                switch (third) {
                    case 1:  // 左上
                        third_column_ = 1;
                        third_row_ = 0;
                        break;
                    case 2:  // 右上
                        third_column_ = 1;
                        third_row_ = 1;
                        break;
                    case 3:  // 左下
                        third_column_ = 0;
                        third_row_ = 0;
                        break;
                    case 4:  // 右下
                        third_column_ = 0;
                        third_row_ = 1;
                        break;
                    default:
                        is_valid_ = false;
                        break;
                }
                return;
            }

            if (level_ == StandardMapGridLevel::Level1000) {
                // 基点は左下（4A）
                third_column_ = 4 - std::stoi(code_.substr(6, 1));  // 0-4
                third_row_ = code_.substr(7, 1)[0] - 'A';  // 0-4 (A-E)

                // Level1000の範囲チェック
                if (third_column_ < 0 || third_column_ > 4 ||
                    third_row_ < 0 || third_row_ > 4) {
                    is_valid_ = false;
                }
                return;
            }

            // Level500
            // 基点は左下（9A）
            third_column_ = 9 - std::stoi(code_.substr(6, 1));  // 0-9
            third_row_ = std::stoi(code_.substr(7, 1));  // 0-9

        } catch (const std::exception& e) {
            is_valid_ = false;
            std::cerr << "Failed to parse grid code " << code_ << ": " << e.what() << std::endl;
        }
    }

    std::string StandardMapGrid::get() const {
        return code_;
    }

    std::pair<TVec3d, TVec3d> StandardMapGrid::calculateGridExtent() const {
        // 東西方向のインデックスを計算（東がプラス、西がマイナス）
        int row_index = first_row_ - 'E';

        // 南北方向のインデックスを計算（北がプラス、南がマイナス）
        int column_index = 'J' - first_column_;

        // Level50000の計算
        // column座標（南北）の計算
        double min_column = column_index * level50000_cell_column;
        double max_column = min_column + level50000_cell_column;

        // row座標（東西）の計算
        double min_row = row_index * level50000_cell_row;
        double max_row = min_row + level50000_cell_row;

        if (level_ == StandardMapGridLevel::Level50000) {
            return createExtentPair(min_column, min_row, max_column, max_row);
        }

        // Level5000の計算
        calculateSubGridExtent(
            min_column, min_row, max_column, max_row,
            second_column_, second_row_,
            level5000_cell_column, level5000_cell_row);

        if (level_ == StandardMapGridLevel::Level5000) {
            return createExtentPair(min_column, min_row, max_column, max_row);
        }

        if (level_ == StandardMapGridLevel::Level2500) {

            calculateSubGridExtent(
                min_column, min_row, max_column, max_row,
                third_column_, third_row_,
                level2500_cell_column, level2500_cell_row);
            return createExtentPair(min_column, min_row, max_column, max_row);

        } else if (level_ == StandardMapGridLevel::Level1000) {

            calculateSubGridExtent(
                min_column, min_row, max_column, max_row,
                third_column_, third_row_,
                level1000_cell_column, level1000_cell_row);
            return createExtentPair(min_column, min_row, max_column, max_row);

        } else {  // Level500

            calculateSubGridExtent(
                min_column, min_row, max_column, max_row,
                third_column_, third_row_,
                level500_cell_column, level500_cell_row);
            return createExtentPair(min_column, min_row, max_column, max_row);
        }
    }

    geometry::Extent StandardMapGrid::getExtent() const {
        if (!isValid()) {
            throw std::runtime_error("Invalid standard map grid code.");
        }

        const auto [planeMin, planeMax] = calculateGridExtent();

        // GeoReferenceを取得
        plateau::polygonMesh::MeshExtractOptions options;
        options.coordinate_zone_id = coordinate_origin_;
        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // 平面直角座標系から緯度経度に変換
        const auto min_coordinate = geo_reference.unproject(planeMin);
        const auto max_coordinate = geo_reference.unproject(planeMax);

        return geometry::Extent(min_coordinate, max_coordinate);
    }

    bool StandardMapGrid::isValid() const {
        return is_valid_;
    }

    std::shared_ptr<GridCode> StandardMapGrid::upper() const {
        // １段階上のレベルの図郭コードに変換
        auto new_grid_code = std::shared_ptr<GridCode>(upperRaw());
        return new_grid_code;
    }

    GridCode* StandardMapGrid::upperRaw() const {
        auto* new_grid = new StandardMapGrid(code_);

        switch (level_) {
            case StandardMapGridLevel::Level500:
            case StandardMapGridLevel::Level1000:
            case StandardMapGridLevel::Level2500:
                // 6桁のLevel5000コードにする（末尾を削除）
                new_grid->code_ = code_.substr(0, 6);
                new_grid->level_ = StandardMapGridLevel::Level5000;
                break;

            case StandardMapGridLevel::Level5000:
                // 4桁のLevel50000コードにする（末尾を削除）
                new_grid->code_ = code_.substr(0, 4);
                new_grid->level_ = StandardMapGridLevel::Level50000;
                break;

            case StandardMapGridLevel::Level50000:
            default:
                // 不正なレベルの場合も無効にする
                new_grid->is_valid_ = false;
                break;
        }

        return new_grid;
    }

    int StandardMapGrid::getLevel() const {
        return (int)level_;
    }

    bool StandardMapGrid::isLargestLevel() const {
        return level_ == StandardMapGridLevel::Level50000;
    }

    bool StandardMapGrid::isSmallerThanNormalGml() const {
        return  level_ < StandardMapGridLevel::Level2500;
    }

    bool StandardMapGrid::isNormalGmlLevel() const {
        return level_ == StandardMapGridLevel::Level2500;
    }

    bool StandardMapGrid::operator==(const StandardMapGrid& other) const {
        return code_ == other.code_;
    }

    bool StandardMapGrid::operator<(const StandardMapGrid& other) const {
        return code_ < other.code_;
    }
} 