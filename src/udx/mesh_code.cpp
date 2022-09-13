#include <iomanip>
#include <stdexcept>
#include <sstream>

#include <plateau/udx/mesh_code.h>

namespace plateau::udx {
    namespace {
        constexpr int second_division_count = 8;
        constexpr int third_division_count = 10;

        constexpr double first_cell_height = 2.0 / 3.0;
        constexpr double first_cell_width = 1.0;
        constexpr double second_cell_height = first_cell_height / second_division_count;
        constexpr double second_cell_width = first_cell_width / second_division_count;
        constexpr double third_cell_height = second_cell_height / third_division_count;
        constexpr double third_cell_width = second_cell_width / third_division_count;
    }

    MeshCode::MeshCode(const std::string& code)
        : level_(getLevel(code)) {
        first_row_ = static_cast<int>(std::stoi(code.substr(0, 2)));
        first_col_ = static_cast<int>(std::stoi(code.substr(2, 2)));
        second_row_ = static_cast<int>(std::stoi(code.substr(4, 1)));
        second_col_ = static_cast<int>(std::stoi(code.substr(5, 1)));

        if (level_ == 2)
            return;

        third_row_ = static_cast<int>(std::stoi(code.substr(6, 1)));
        third_col_ = static_cast<int>(std::stoi(code.substr(7, 1)));
    }

    int MeshCode::getLevel(const std::string& code) {
        if (code.size() == 6) {
            return 2;
        }
        if (code.size() == 8) {
            return 3;
        }
        // 2次メッシュ、3次メッシュ以外はサポート対象外
        throw std::runtime_error("Invalid string for regional mesh code");
    }

    geometry::Extent MeshCode::getExtent() const {
        geometry::GeoCoordinate min(0.0, 0.0, 0.0);
        min.latitude = first_row_ * first_cell_height;
        min.longitude = first_col_ * first_cell_width + 100;

        min.latitude += second_row_ * second_cell_height;
        min.longitude += second_col_ * second_cell_width;

        if (level_ == 2) {
            auto max = min;
            max.latitude += second_cell_height;
            max.longitude += second_cell_width;
            return { min, max };
        }

        min.latitude += third_row_ * third_cell_height;
        min.longitude += third_col_ * third_cell_width;

        auto max = min;
        max.latitude += third_cell_height;
        max.longitude += third_cell_width;
        return { min, max };
    }

    MeshCode MeshCode::getThirdMesh(const geometry::GeoCoordinate& coordinate) {
        auto sub_coordinate = coordinate;
        sub_coordinate.longitude -= 100;

        MeshCode result;
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

    bool MeshCode::isWithin(const MeshCode& other) const
    {
        if (get() == other.get())
            return true;

        return get().substr(0, 6) == other.get();
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
        return oss.str();
    }

    bool MeshCode::operator==(const MeshCode& other) const {
        return get() == other.get();
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

        return 0;
    }

    int MeshCode::compareRow(const MeshCode& lhs, const MeshCode& rhs) {
        if (lhs.first_row_ > rhs.first_row_) return 1;
        if (lhs.first_row_ < rhs.first_row_) return -1;

        if (lhs.second_row_ > rhs.second_row_) return 1;
        if (lhs.second_row_ < rhs.second_row_) return -1;

        if (lhs.third_row_ > rhs.third_row_) return 1;
        if (lhs.third_row_ < rhs.third_row_) return -1;

        return 0;
    }
}
