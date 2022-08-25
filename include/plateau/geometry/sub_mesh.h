#pragma once

#include <string>
#include <vector>

namespace plateau::geometry{
    /**
     * SubMesh は Mesh によって所有されます。
     * Mesh の一部 (Indices リストの中のとある範囲)がとあるテクスチャであることを表現します。
     *
     * 詳しくは Model クラスのコメントをご覧ください。
     */
    struct SubMesh{
        SubMesh(int startIndex, int endIndex, std::string texturePath);
        /**
         * 引数で与えられた SubMesh の vector に SubMesh を追加します。
         */
        static void addSubMesh(size_t startIndex, size_t endIndex, std::string texturePath, std::vector<SubMesh> &vector);
        /**
         * startIndex, endIndex は、Meshの Indices リストの中のある範囲を表現します。
         * 範囲は [start, end] (endを範囲に含む) です。
         */
        size_t startIndex;
        size_t endIndex;
        std::string texturePath;
    };
}