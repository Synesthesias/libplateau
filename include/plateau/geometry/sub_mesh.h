#pragma once

#include <string>

namespace plateau::geometry{
    /**
     * SubMesh は Mesh によって所有されます。
     * Mesh の一部 (Indices リストの中のとある範囲)がとあるテクスチャであることを表現します。
     */
    struct SubMesh{
        /**
         * startIndex, endIndex は、Meshの Indices リストの中のある範囲を表現します。
         */
        int startIndex;
        int endIndex;
        std::string texturePath;
    };
}