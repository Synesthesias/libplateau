
#pragma once

#include <plateau/polygon_mesh/model.h>
#include <plateau/texture/image_reader.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

using namespace plateau::polygonMesh;
using namespace image_reader;

namespace plateau::texture {

class AtlasInfo {
public:

    explicit AtlasInfo() : valid(false), left(0), top(0), width(0), height(0), uPos(0), vPos(0), uFactor(0), vFactor(0) {}
    ~AtlasInfo() {}

    size_t getLeft() const { return left; }
    size_t getTop() const { return top; }
    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    double getUPos() const { return uPos; }
    double getVPos() const { return vPos; }
    double getUFactor() const { return uFactor; }
    double getVFactor() const { return vFactor; }

    bool getValid() const;
    void clear();
    void set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height,
        double uPos, double vPos, double uFactor, double vFactor);

private:
    bool valid;     // パッキングが成功、失敗のフラグ（TRUE:成功、FALSE:失敗）
    size_t left;    // パッキングされた画像の左上のX座標
    size_t top;     // パッキングされた画像の左上のY座標
    size_t width;   // パッキングされた画像の幅
    size_t height;  // パッキングされた画像の高さ
    double uPos;    // uv座標の左上u座標
    double vPos;    // uv座標の左上v座標
    double uFactor; // u座標の倍率
    double vFactor; // v座標の倍率
};

class AtlasContainer {
public:

    explicit AtlasContainer(const size_t _gap, const size_t _horizontal_range, const size_t _vertical_range);
    ~AtlasContainer() {}

    size_t getGap() const { return gap; }
    size_t getRootHorizontalRange() const { return root_horizontal_range; }
    size_t getHorizontalRange() const { return horizontal_range; }
    size_t getVerticalRange() const { return vertical_range; }
    void add(const size_t _length);

private:
    size_t gap;                     // 画像を格納するコンテナの高さ
    size_t root_horizontal_range;   // コンテナ内で未使用の領域のX座標
    size_t horizontal_range;        // 最後の画像をパッキングするための領域のX座標
    size_t vertical_range;          // パッキングの対象となる親の画像のコンテナが配置されている左上のY座標
};

class TexturePacker {
public:
    const int defaultResolution = 2048;
    const unsigned char gray = 80;

    explicit TexturePacker() : canvas_width(0), canvas_height(0), vertical_range(0), capacity(0), coverage(0) {
    }
    explicit TexturePacker(size_t width, size_t height) : vertical_range(0), capacity(0), coverage(0) {
        canvas_width = width;
        canvas_height = height;
        canvas.init(width, height, gray);
    }
    ~TexturePacker() {
    }

    void setSaveFilePath(std::string fileName);

    void init(size_t width, size_t height);
    void clear();
    void flush();

    void process(Model& model);
    void processNodeRecursive(const Node& node);

    double getCoverage() const {
        return coverage;
    } // 呼び出し時のテクスチャ全体に対しての既にパックされた画像の占有率（100%）
    void update(const size_t _width, const size_t _height, const bool _is_new_container); // 画像のパッキング成功時の処理、第3引数（TRUE:新規コンテナを作成、FALSE:既存コンテナに追加）
    const AtlasInfo insert(const size_t _width, const size_t _height); // 指定された画像領域（width x height）の領域が確保できるか検証、戻り値AtrasInfoの「valid」ブール値（true:成功、false:失敗）で判定可能

private:
    static int serialNumber;
    std::vector<AtlasContainer> container_list;
    AtlasInfo atlas_info;
    size_t canvas_width;
    size_t canvas_height;
    size_t vertical_range;
    size_t capacity;
    double coverage;
    TextureImage canvas;
    std::filesystem::path saveFilePath;
};
} // namespace plateau::texture


