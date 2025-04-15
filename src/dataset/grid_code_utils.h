#pragma once

#include <set>
#include <string>
#include <vector>
#include <plateau/dataset/grid_code.h>

namespace plateau::dataset::utils {

/**
 * @brief GridCodeのベクタを受け取り、それらの上位レベルを含むグリッドコード文字列のセットを生成します。
 * @param grid_codes GridCodeへのポインタのベクタ。
 * @return 上位レベルを含むグリッドコード文字列のセット。不正なGridCodeは無視されます。
 */
    std::set<std::string> createExpandedGridCodeSet(const std::vector<GridCode*>& grid_codes);

} 