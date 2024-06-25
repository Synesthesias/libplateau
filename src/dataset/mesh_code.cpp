#include <iomanip>
#include <stdexcept>
#include <sstream>

#include <plateau/dataset/mesh_code.h>
#include <algorithm>

namespace plateau::dataset {
    namespace {
        constexpr int second_division_count = 8;
        constexpr int third_division_count = 10;

        // 1次メッシュの緯度長
        constexpr double first_cell_height = 2.0 / 3.0;
        // 1次メッシュの経度長
        constexpr double first_cell_width = 1.0;
        // 2次メッシュの緯度長
        constexpr double second_cell_height = first_cell_height / second_division_count;
        // 2次メッシュの経度長
        constexpr double second_cell_width = first_cell_width / second_division_count;
        // 3次メッシュの緯度長
        constexpr double third_cell_height = second_cell_height / third_division_count;
        // 3次メッシュの経度長
        constexpr double third_cell_width = second_cell_width / third_division_count;
        // 4次メッシュの緯度長
        constexpr double fourth_cell_height = third_cell_height / 2.0;
        // 4次メッシュの経度長
        constexpr double fourth_cell_width = third_cell_width / 2.0;
        // 5次メッシュの緯度長
        constexpr double fifth_cell_height = fourth_cell_height / 2.0;
        // 5次メッシュの経度長
        constexpr double fifth_cell_width = fourth_cell_width / 2.0;
    }

    namespace {
        /**
         * メッシュコードの文字列からレベル（詳細度）を返します。
         * 文字列が不正な場合は -1 を返します。
         */
        int parseLevel(const std::string& code) {
            if (code.size() == 6) {
                return 2;
            }
            if (code.size() == 8) {
                return 3;
            }
            if (code.size() == 9) {
                return 4;
            }
            if (code.size() == 10) {
                return 5;
            }

            // 6次メッシュ以上はサポート対象外
            return -1;
        }

        /**
         * \brief 2分割メッシュ(4次メッシュ以降)の番号からメッシュの行番号、列番号を計算します。
         */
        bool getHalfMeshCellPosition(int num, int& out_row, int& out_col) {
            // 番号順に左下→右下→左上→右上
            switch (num) {
            case 1:
                out_row = 0;
                out_col = 0;
                return true;
            case 2:
                out_row = 0;
                out_col = 1;
                return true;
            case 3:
                out_row = 1;
                out_col = 0;
                return true;
            case 4:
                out_row = 1;
                out_col = 1;
                return true;
            default:
                return false;
            }
        }


        /**
         * \brief 2分割メッシュ(4次メッシュ以降)のメッシュの行番号、列番号からメッシュ番号を計算します。
         */
        bool getHalfMeshNumber(int& out_num, int row, int col) {
            if (row < 0 || row > 1 ||
                col < 0 || col > 1)
                return false;

            // 番号順に左下→右下→左上→右上
            out_num = row * 2 + col + 1;
            return true;
        }
    }

    MeshCode::MeshCode(const std::string& code) {
        // メッシュコードの文字列が数字のみからなることをチェックします。
        if (!std::all_of(code.cbegin(), code.cend(), isdigit)) {
            is_valid_ = false;
            return;
        }

        // メッシュコードのレベル（詳細度）をチェックします。
        is_valid_ = true;
        level_ = parseLevel(code);
        if (level_ < 0) {
            is_valid_ = false;
            return;
        }

        first_row_ = std::stoi(code.substr(0, 2));
        first_col_ = std::stoi(code.substr(2, 2));
        second_row_ = std::stoi(code.substr(4, 1));
        second_col_ = std::stoi(code.substr(5, 1));

        if (level_ == 2)
            return;

        third_row_ = std::stoi(code.substr(6, 1));
        third_col_ = std::stoi(code.substr(7, 1));

        if (level_ == 3)
            return;

        const auto fourth_num = std::stoi(code.substr(8, 1));
        getHalfMeshCellPosition(fourth_num, fourth_row_, fourth_col_);

        if (level_ == 4)
            return;

        const auto fifth_num = std::stoi(code.substr(9, 1));
        getHalfMeshCellPosition(fifth_num, fifth_row_, fifth_col_);
    }

    geometry::Extent MeshCode::getExtent() const {
        // 1次メッシュの左下の座標を計算
        geometry::GeoCoordinate min(0.0, 0.0, 0.0);
        // 地域メッシュは(0, 100)からスタート
        min.latitude = first_row_ * first_cell_height;
        min.longitude = first_col_ * first_cell_width + 100;

        // 2次メッシュの左下の座標を計算
        min.latitude += second_row_ * second_cell_height;
        min.longitude += second_col_ * second_cell_width;

        if (level_ == 2) {
            // 2次メッシュの右上の座標を計算
            auto max = min;
            max.latitude += second_cell_height;
            max.longitude += second_cell_width;
            return { min, max };
        }

        // 3次メッシュの左下の座標を計算
        min.latitude += third_row_ * third_cell_height;
        min.longitude += third_col_ * third_cell_width;

        if (level_ == 3) {
            // 3次メッシュの右上の座標を計算
            auto max = min;
            max.latitude += third_cell_height;
            max.longitude += third_cell_width;
            return { min, max };
        }

        // 4次メッシュの左下の座標を計算
        min.latitude += fourth_row_ * fourth_cell_height;
        min.longitude += fourth_col_ * fourth_cell_width;

        if (level_ == 4) {
            // 4次メッシュの右上の座標を計算
            auto max = min;
            max.latitude += fourth_cell_height;
            max.longitude += fourth_cell_width;
            return { min, max };
        }

        // 5次メッシュの左下の座標を計算
        min.latitude += fifth_row_ * fifth_cell_height;
        min.longitude += fifth_col_ * fifth_cell_width;

        // 5次メッシュの右上の座標を計算
        auto max = min;
        max.latitude += fifth_cell_height;
        max.longitude += fifth_cell_width;
        return { min, max };
    }

    MeshCode MeshCode::getThirdMesh(const geometry::GeoCoordinate& coordinate) {
        auto sub_coordinate = coordinate;
        sub_coordinate.longitude -= 100;

        MeshCode result{};
        result.level_ = 3;

        result.first_row_ = static_cast<int>(sub_coordinate.latitude / first_cell_height);
        result.first_col_ = static_cast<int>(sub_coordinate.longitude / first_cell_width);

        sub_coordinate.latitude -= result.first_row_ * first_cell_height;
        sub_coordinate.longitude -= result.first_col_ * first_cell_width;

        result.second_row_ = static_cast<int>(sub_coordinate.latitude / second_cell_height);
        result.second_col_ = static_cast<int>(sub_coordinate.longitude / second_cell_width);

        sub_coordinate.latitude -= result.second_row_ * second_cell_height;
        sub_coordinate.longitude -= result.second_col_ * second_cell_width;

        result.third_row_ = static_cast<int>(sub_coordinate.latitude / third_cell_height);
        result.third_col_ = static_cast<int>(sub_coordinate.longitude / third_cell_width);

        return result;
    }

    void MeshCode::getThirdMeshes(const geometry::Extent& extent, std::vector<MeshCode>& mesh_codes) {
        auto min_mesh = getThirdMesh(extent.min);
        const auto max_mesh = getThirdMesh(extent.max);
        while (compareRow(max_mesh, min_mesh) >= 0) {
            auto mesh = min_mesh;
            while (compareCol(max_mesh, mesh) >= 0) {
                mesh_codes.push_back(mesh);
                nextCol(mesh);
            }
            nextRow(min_mesh);
        }
    }

    std::shared_ptr<std::vector<MeshCode>> MeshCode::getThirdMeshes(const geometry::Extent& extent) {
        auto result = std::make_shared<std::vector<MeshCode>>();
        getThirdMeshes(extent, *result);
        return result;
    }

    bool MeshCode::isWithin(const MeshCode& other) const {
        if (get() == other.get())
            return true;

        return get().substr(0, 6) == other.get();
    }

    MeshCode MeshCode::asSecond() const {
        auto result = *this;
        result.level_ = 2;
        return result;
    }

    MeshCode& MeshCode::upper() {
        // レベル2以上の範囲で１段階上のレベルの地域メッシュに変換
        level_ = std::max(1, level_ - 1);
        if (level_ < 2)
            is_valid_ = false;

        return *this;
    }

    std::string MeshCode::get() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << first_row_;
        oss << std::setfill('0') << std::setw(2) << first_col_;
        oss << std::setw(1) << second_row_;
        oss << std::setw(1) << second_col_;

        if (level_ == 2)
            return oss.str();

        oss << std::setw(1) << third_row_;
        oss << std::setw(1) << third_col_;

        if (level_ == 3)
            return oss.str();

        int fourth_num;
        getHalfMeshNumber(fourth_num, fourth_row_, fourth_col_);
        oss << std::setw(1) << fourth_num;

        if (level_ == 4)
            return oss.str();

        int fifth_num;
        getHalfMeshNumber(fifth_num, fifth_row_, fifth_col_);
        oss << std::setw(1) << fifth_num;

        return oss.str();
    }

    int MeshCode::getLevel() const {
        return level_;
    }

    bool MeshCode::isValid() const {
        return is_valid_;
    }

    bool MeshCode::operator==(const MeshCode& other) const {
        return get() == other.get();
    }

    bool MeshCode::operator<(MeshCode& other) const {
        return std::stoi(get()) < std::stoi(other.get());
    }

    bool MeshCode::operator<(const MeshCode& other) const {
        return std::stoi(get()) < std::stoi(other.get());
    }

    void MeshCode::nextCol(MeshCode& mesh_code) {
        if (mesh_code.third_col_ < third_division_count - 1) {
            mesh_code.third_col_ += 1;
            return;
        }
        mesh_code.third_col_ = 0;

        if (mesh_code.second_col_ < second_division_count - 1) {
            mesh_code.second_col_ += 1;
            return;
        }
        mesh_code.second_col_ = 0;

        mesh_code.first_col_ += 1;
    }

    void MeshCode::nextRow(MeshCode& mesh_code) {
        if (mesh_code.third_row_ < third_division_count - 1) {
            mesh_code.third_row_ += 1;
            return;
        }
        mesh_code.third_row_ = 0;

        if (mesh_code.second_row_ < second_division_count - 1) {
            mesh_code.second_row_ += 1;
            return;
        }
        mesh_code.second_row_ = 0;

        mesh_code.first_row_ += 1;
    }

    int MeshCode::compareCol(const MeshCode& lhs, const MeshCode& rhs) {
        if (lhs.first_col_ > rhs.first_col_) return 1;
        if (lhs.first_col_ < rhs.first_col_) return -1;

        if (lhs.second_col_ > rhs.second_col_) return 1;
        if (lhs.second_col_ < rhs.second_col_) return -1;

        if (lhs.third_col_ > rhs.third_col_) return 1;
        if (lhs.third_col_ < rhs.third_col_) return -1;

        if (lhs.fourth_col_ > rhs.fourth_col_) return 1;
        if (lhs.fourth_col_ < rhs.fourth_col_) return -1;

        if (lhs.fifth_col_ > rhs.fifth_col_) return 1;
        if (lhs.fifth_col_ < rhs.fifth_col_) return -1;

        return 0;
    }

    int MeshCode::compareRow(const MeshCode& lhs, const MeshCode& rhs) {
        if (lhs.first_row_ > rhs.first_row_) return 1;
        if (lhs.first_row_ < rhs.first_row_) return -1;

        if (lhs.second_row_ > rhs.second_row_) return 1;
        if (lhs.second_row_ < rhs.second_row_) return -1;

        if (lhs.third_row_ > rhs.third_row_) return 1;
        if (lhs.third_row_ < rhs.third_row_) return -1;

        if (lhs.fourth_row_ > rhs.fourth_row_) return 1;
        if (lhs.fourth_row_ < rhs.fourth_row_) return -1;

        if (lhs.fifth_row_ > rhs.fifth_row_) return 1;
        if (lhs.fifth_row_ < rhs.fifth_row_) return -1;

        return 0;
    }
}
