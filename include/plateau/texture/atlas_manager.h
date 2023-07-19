
#pragma once

#include <libplateau_api.h>
#include <vector>

struct jpeg_error_mgr;

namespace plateau::texture {

    class LIBPLATEAU_EXPORT AtlasInfo {
    public:

        explicit AtlasInfo() : valid(false), left(0), top(0), width(0), height(0) {
        }
        ~AtlasInfo() = default;

        size_t getLeft() const {
            return left;
        }
        size_t getTop() const {
            return top;
        }
        size_t getWidth() const {
            return width;
        }
        size_t getHeight() const {
            return height;
        }

        bool getValid() const;
        void clear();
        void set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height);

    private:
        bool valid;     // パッキングが成功、失敗のフラグ（TRUE:成功、FALSE:失敗）
        size_t left;    // パッキングされた画像の左上のX座標
        size_t top;     // パッキングされた画像の左上のY座標
        size_t width;   // パッキングされた画像の幅
        size_t height;  // パッキングされた画像の高さ
    };

    class LIBPLATEAU_EXPORT AtlasContainer {
    public:

        explicit AtlasContainer(const size_t _gap, const size_t _horizontal_range, const size_t _vertical_range);
        ~AtlasContainer() = default;

        size_t getGap() const {
            return gap;
        }
        size_t getRootHorizontalRange() const {
            return root_horizontal_range;
        }
        size_t getHorizontalRange() const {
            return horizontal_range;
        }
        size_t getVerticalRange() const {
            return vertical_range;
        }
        void add(const size_t _length);

    public:
        static int counter;

    private:
        size_t gap;                     // 画像を格納するコンテナの高さ
        size_t root_horizontal_range;   // コンテナ内で未使用の領域のX座標
        size_t horizontal_range;        // 最後の画像をパッキングするための領域のX座標
        size_t vertical_range;          // パッキングの対象となる親の画像のコンテナが配置されている左上のY座標
    };

    class LIBPLATEAU_EXPORT AtlasManager {
    public:

        explicit AtlasManager(const size_t _width, const size_t _height);
        ~AtlasManager() = default;

        double getCoverage() const {
            return coverage;
        } // 呼び出し時のテクスチャ全体に対しての既にパックされた画像の占有率（100%）
        void update(const size_t _width, const size_t _height, const bool _is_new_container); // 画像のパッキング成功時の処理、第3引数（TRUE:新規コンテナを作成、FALSE:既存コンテナに追加）
        const AtlasInfo insert(const size_t _width, const size_t _height); // 指定された画像領域（width x height）の領域が確保できるか検証、戻り値AtrasInfoの「valid」ブール値（true:成功、false:失敗）で判定可能

    private:
        std::vector<AtlasContainer> container_list;
        AtlasInfo atlas_info;
        size_t canvas_width;
        size_t canvas_height;
        size_t vertical_range;
        size_t capacity;
        double coverage;
    };

}
