#include "plateau/dataset/standard_map_grid.h"

#include <algorithm>

#include "plateau/geometry/geo_coordinate.h"
#include <cctype>
#include <stdexcept>
#include <plateau/geometry/geo_reference.h>
#include <plateau/polygon_mesh/mesh_extract_options.h>

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
        constexpr double level50000_cell_width = 40000.0;  // 40km
        constexpr double level50000_cell_height = 30000.0; // 30km
        constexpr int level50000_row_count = 20; // 縦方向の分割数

        constexpr double level5000_cell_height = level50000_cell_height / level5000_division_count; // 3km
        constexpr double level5000_cell_width = level50000_cell_width / level5000_division_count; // 4km

        constexpr double level2500_cell_height = level5000_cell_height / level2500_division_count; // 1.5km
        constexpr double level2500_cell_width = level5000_cell_width / level2500_division_count; // 2km

        constexpr double level1000_cell_height = level5000_cell_height / level1000_division_count; // 600m
        constexpr double level1000_cell_width = level5000_cell_width / level1000_division_count; // 800m

        constexpr double level500_cell_height = level5000_cell_height / level500_division_count; // 300m
        constexpr double level500_cell_width = level5000_cell_width / level500_division_count; // 400m

        // 原点から端までのセル数
        constexpr double row_half_count = 10.0;
        constexpr double col_half_count = 4.0;
    }

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

    StandardMapGrid::StandardMapGrid(const std::string& code) : code_(code), is_valid_(true) {
        // 図郭コードの文字列が数字とアルファベットからなることをチェックします。
        if (!std::all_of(code_.begin(), code_.end(), [](char c)
        {
            return std::isalnum(c);
        })) {
            is_valid_ = false;
            return;
        }

        // 図郭コードのレベル（詳細度）をチェックします。
        level_ = parseLevel(code);
        if (level_ == StandardMapGridLevel::Invalid) {
            is_valid_ = false;
            return;
        }

        // 原点設定
        coordinate_origin_ = std::stoi(code.substr(0, 2));

        // 図郭コードの行番号、列番号を取得します。
        first_row_ = code[2];  // 1文字目は行
        first_col_ = code[3];  // 2文字目は列
        if (level_ == StandardMapGridLevel::Level50000) {
            return;
        }

        second_row_ = std::stoi(code.substr(4, 1));
        second_col_ = std::stoi(code.substr(5, 1));

        if (level_ == StandardMapGridLevel::Level5000) {
            return;
        }

        if (level_ == StandardMapGridLevel::Level2500) {
            // 2×2分割のインデックスを計算
            int third = std::stoi(code.substr(6, 1));  // 1-4
            third_row_ = (third - 1) / 2;  // 0 or 1
            third_col_ = (third - 1) % 2;  // 0 or 1
            return;
        }

        if (level_ == StandardMapGridLevel::Level1000) {
            third_row_ = std::stoi(code.substr(6, 1));  // 0-4
            third_col_ = code.substr(7, 1)[0] - 'A';  // 0-4 (A-E)
            return;
        }

        // Level500
        third_row_ = std::stoi(code.substr(6, 1));  // 0-9
        third_col_ = std::stoi(code.substr(7, 1));  // 0-9
    }

    std::string StandardMapGrid::get() const {
        return code_;
    }

    std::pair<TVec3d, TVec3d> StandardMapGrid::calculateGridExtent() const {
        double min_x = 0.0, min_y = 0.0;
        double max_x = 0.0, max_y = 0.0;

        // 親セルの位置に基づいて方向フラグを設定（スコープ外でも使用するため、ここで定義）
        bool is_x_right = false;
        bool is_y_upper = false;

        // Level50000の計算
        {
            // 南北方向（行）のインデックス計算
            // A行から数えて何番目かを計算（0始まり）
            int row_index = first_row_ - 'A';
            
            // 東西方向（列）のインデックス計算
            // A列から数えて何番目かを計算（0始まり）
            int col_index = first_col_ - 'A';

            // 基準点からの距離を計算
            // 列（X座標）: 原点から左右対称（左が負、右が正）
            min_x = (col_index - col_half_count) * level50000_cell_width;
            max_x = min_x + level50000_cell_width;

            // 行（Y座標）: 原点から上下対称（上が正、下が負）
            min_y = (row_half_count - row_index) * level50000_cell_height;
            max_y = min_y + level50000_cell_height;

            // 親セルの位置に基づいて方向フラグを設定
            is_x_right = col_index >= col_half_count;   // E列より右側
            is_y_upper = row_index <= row_half_count;   // 中央より上側
        }

        if (level_ == StandardMapGridLevel::Level50000) {
            return {TVec3d(min_x, min_y, 0), TVec3d(max_x, max_y, 0)};
        }

        // Level5000の計算
        calculateSubGridExtent(
            min_x, min_y, max_x, max_y,
            second_row_, second_col_,
            level5000_cell_width, level5000_cell_height,
            is_x_right, is_y_upper);

        if (level_ == StandardMapGridLevel::Level5000) {
            return {TVec3d(min_x, min_y, 0), TVec3d(max_x, max_y, 0)};
        }

        if (level_ == StandardMapGridLevel::Level2500) {
            calculateSubGridExtent(
                min_x, min_y, max_x, max_y,
                third_row_, third_col_,
                level2500_cell_width, level2500_cell_height,
                is_x_right, is_y_upper);
            return {TVec3d(min_x, min_y, 0), TVec3d(max_x, max_y, 0)};
        } else if (level_ == StandardMapGridLevel::Level1000) {
            calculateSubGridExtent(
                min_x, min_y, max_x, max_y,
                third_row_, third_col_,
                level1000_cell_width, level1000_cell_height,
                is_x_right, is_y_upper);
            return {TVec3d(min_x, min_y, 0), TVec3d(max_x, max_y, 0)};
        } else {  // Level500
            calculateSubGridExtent(
                min_x, min_y, max_x, max_y,
                third_row_, third_col_,
                level500_cell_width, level500_cell_height,
                is_x_right, is_y_upper);
            return {TVec3d(min_x, min_y, 0), TVec3d(max_x, max_y, 0)};
        }
    }

    void StandardMapGrid::calculateSubGridExtent(
        double& min_x, double& min_y,
        double& max_x, double& max_y,
        int row_index, int col_index,
        double cell_width, double cell_height,
        bool is_x_right, bool is_y_upper) const {

        // X座標の計算
        // is_x_rightがtrueの場合、親セルの左端から右方向に移動
        // is_x_rightがfalseの場合、親セルの左端から左方向に移動
        min_x = min_x + (is_x_right ? col_index * cell_width : -col_index * cell_width);
        max_x = min_x + cell_width;

        // Y座標の計算
        // is_y_upperがtrueの場合、親セルの下端から上方向に移動
        // is_y_upperがfalseの場合、親セルの下端から下方向に移動
        min_y = min_y + (is_y_upper ? row_index * cell_height : -row_index * cell_height);
        max_y = min_y + cell_height;
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

    bool StandardMapGrid::isWithin(const GridCode& other) const {
        if (!isValid()) return false;
        
        // 同じ型の場合のみ比較
        const auto* other_grid = dynamic_cast<const StandardMapGrid*>(&other);
        if (other_grid == nullptr) return false;

        if (code_ == other_grid->code_) {
            return true;
        }

        // Level50000の場合は先頭4文字、Level50000以外の場合は先頭6文字で比較
        const size_t compare_length = (level_ == StandardMapGridLevel::Level50000) ? 4 : 6;
        return code_.substr(0, compare_length) == other_grid->code_.substr(0, compare_length);
    }

    bool StandardMapGrid::isValid() const {
        return is_valid_;
    }

    std::shared_ptr<GridCode> StandardMapGrid::upper() {
        // １段階上のレベルの図郭コードに変換
        auto new_code = std::make_shared<StandardMapGrid>(code_);
        new_code->level_ = static_cast<StandardMapGridLevel>(static_cast<int>(level_) - 1);
        new_code->is_valid_ = new_code->level_ >= StandardMapGridLevel::Level50000;
        return new_code;
    }

    int StandardMapGrid::getLevel() const {
        return (int)level_;
    }

    bool StandardMapGrid::isLargestLevel() const {
        return level_ == StandardMapGridLevel::Level50000;
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