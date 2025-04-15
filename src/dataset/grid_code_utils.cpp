#include "grid_code_utils.h"

namespace plateau::dataset::utils {

    std::set<std::string> createExpandedGridCodeSet(const std::vector<GridCode*>& grid_codes) {
        std::set<std::string> grid_codes_str_set;
        for (const auto& grid_code_ptr: grid_codes) {
            if (!grid_code_ptr || !grid_code_ptr->isValid()) {
                continue; // 不正なポインタやコードはスキップ
            }

            // createは毎回新しいインスタンスを生成するため、ループの外で取得する
            std::shared_ptr<GridCode> current_code = GridCode::create(grid_code_ptr->get());
            if (!current_code || !current_code->isValid()) {
                continue;
            }

            // 現在のコードとその上位コードをセットに追加
            while (true) {
                if (!current_code->isValid()) break;
                grid_codes_str_set.insert(current_code->get());
                if (current_code->isLargestLevel()) break; // 最上位なら終了
                current_code = current_code->upper();
                if (!current_code) break;
            }
        }
        return grid_codes_str_set;
    }
}