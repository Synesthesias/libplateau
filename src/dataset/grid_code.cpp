#include "plateau/dataset/grid_code.h"
#include "plateau/dataset/mesh_code.h"
#include "plateau/dataset/standard_map_grid.h"
#include "plateau/dataset/invalid_grid_code.h"
#include <cctype>
#include <stdexcept>

namespace plateau::dataset {

    GridCode* GridCode::createRaw(const std::string& code) {
        if (code.empty()) {
            return new InvalidGridCode();
        }

        bool has_digit = false;
        bool has_upper = false;

        // コードの文字を検査
        for (char c : code) {
            if (std::isdigit(c)) has_digit = true;
            if (std::isupper(c)) has_upper = true;
        }

        try {
            // 数字のみの場合はメッシュコード
            if (has_digit && !has_upper) {
                return new MeshCode(code);
            }
            // 数字と大文字アルファベットの場合は国土基本図図郭
            else if (has_digit && has_upper) {
                return new StandardMapGrid(code);
            }
        } catch (const std::invalid_argument&) {
            return new InvalidGridCode();
        }

        return new InvalidGridCode();
    }

    std::shared_ptr<GridCode> GridCode::create(const std::string& code) {
        return std::shared_ptr<GridCode>(createRaw(code));
    }

} 