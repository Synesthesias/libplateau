#include "plateau/height_map_generator/height_map_with_alpha.h"
#include <cmath>
#include <memory>

namespace plateau::heightMapGenerator {

    HeightMapWithAlpha::HeightMapWithAlpha(const size_t texture_width, const size_t texture_height, const double cartesian_width, const double cartesian_height) :
            height_map(texture_width * texture_height, 0),
            alpha_map(texture_width * texture_height, 0),
            texture_width(texture_width),
            texture_height(texture_height),
            cartesian_width(cartesian_width),
            cartesian_height(cartesian_height) {

    }

    void HeightMapWithAlpha::setHeightAt(const size_t index, const HeightMapElemT height) {
        height_map.at(index) = height;
    }

    void HeightMapWithAlpha::setAlphaAt(const size_t index, const AlphaMapElemT alpha) {
        alpha_map.at(index) = alpha;
    }

    bool HeightMapWithAlpha::isInvisibleAt(const size_t index) const {
        return alpha_map.at(index) < 0.01;
    }

    bool HeightMapWithAlpha::isOpaqueAt(const size_t index) const {
        return alpha_map.at(index) > 0.99;
    }

    void HeightMapWithAlpha::fillTransparentEdges() {
        struct Pixel {
            int x, y;
            HeightMapElemT color;
        };
        std::vector<Pixel> edgePixels;

        // エッジ検索
        for (int y = 0; y < texture_height; ++y) {
            for (int x = 0; x < texture_width; ++x) {
                if (isOpaqueAt(indexAt(x,y))) {
                    edgePixels.push_back({ x, y, height_map.at(indexAt(x,y)) });
                    break;
                }
            }
            for (int x = (int)texture_width - 1; x >= 0; --x) {
                if (isOpaqueAt(indexAt(x,y))) {
                    edgePixels.push_back({ x, y, height_map.at(indexAt(x,y)) });
                    break;
                }
            }
        }
        for (int x = 0; x < texture_width; ++x) {
            for (int y = 0; y < texture_height; ++y) {
                if (isOpaqueAt(indexAt(x,y))) {
                    edgePixels.push_back({ x, y, height_map.at(indexAt(x,y)) });
                    break;
                }
            }
            for (int y = (int)texture_height - 1; y >= 0; --y) {
                if (isOpaqueAt(indexAt(x,y))) {
                    edgePixels.push_back({ x, y, height_map.at(indexAt(x,y)) });
                    break;
                }
            }
        }

        // アルファ部分を最も距離の近いエッジのピクセル色に
        for (int y = 0; y < texture_height; ++y) {
            for (int x = 0; x < texture_width; ++x) {
                if (isInvisibleAt(indexAt(x,y))) {
                    double minDist = std::numeric_limits<double>::max();
                    HeightMapElemT nearestColor = 0;
                    for (const auto& p : edgePixels) {
                        double dist = std::sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
                        if (dist < minDist) {
                            minDist = dist;
                            nearestColor = p.color;
                        }
                    }
                    height_map.at(indexAt(x,y)) = nearestColor;
                }
            }
        }
    }

    void HeightMapWithAlpha::applyConvolutionFilterForHeightMap() {
        size_t imageSize = texture_width * texture_height;
        std::unique_ptr<HeightMapElemT[]> tempImage = std::make_unique<HeightMapElemT[]>(imageSize);
        memcpy(tempImage.get(), height_map.data(), sizeof(HeightMapElemT) * imageSize);
        //エッジを除外して処理
        for (size_t y = 1; y < texture_height - 1; ++y) {
            for (size_t x = 1; x < texture_width - 1; ++x) {
                // 3x3の領域のピクセル値の平均を計算
                int sum = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        sum += height_map.at(indexAt(x+dx, y+dy));
                    }
                }
                tempImage[indexAt(x,y)] = sum / 9;
            }
        }
        memcpy(height_map.data(), tempImage.get(), sizeof(HeightMapElemT) * imageSize);
    }

    void HeightMapWithAlpha::expandOpaqueArea(double expand_width_cartesian) {
        double pixel_width = cartesian_width / (double)texture_width;
        double pixel_height = cartesian_height / (double)texture_height;
        int expand_width_pixel = std::ceil(expand_width_cartesian / pixel_width);
        int expand_height_pixel = std::ceil(expand_width_cartesian / pixel_height);
        auto next_alpha = AlphaMapT(alpha_map);
        for (int y = 0; y < texture_height; y++) {
            for (int x = 0; x < texture_width; x++) {
                for (int y2 = std::max(y - expand_height_pixel, 0); y2 < y; y2++) {
                    for (int x2 = std::max(x - expand_width_pixel,0); x2 < x; x2++) {
                        next_alpha.at(indexAt(x2,y2)) = std::max(next_alpha.at(indexAt(x2,y2)), getAlphaAt(indexAt(x,y)));
                    }
                }

            }
        }
        alpha_map = std::move(next_alpha);
    }

    void HeightMapWithAlpha::averagingAlphaMap(double averaging_width_cartesian) {
        double pixel_width = cartesian_width / (double)texture_width;
        double pixel_height = cartesian_height / (double)texture_height;
        auto next_alpha = AlphaMapT (alpha_map);

        int averaging_width_pixel = std::floor(averaging_width_cartesian / pixel_width);
        int averaging_height_pixel = std::floor(averaging_width_cartesian / pixel_height);
        for(int y=0; y<texture_height; y++) {
            for(int x=0; x<texture_width; x++) {
                double sum = 0;
                int num = 0;
                // 周辺のピクセルを確認
                for(int y2 = std::max(y - averaging_height_pixel, 0); y2 <= std::min(y + averaging_height_pixel, (int)texture_height - 1); y2++) {
                    for(int x2 = std::max(x - averaging_width_pixel, 0); x2 <= std::min(x + averaging_width_pixel, (int)texture_width - 1); x2++) {
                        sum += alpha_map.at(indexAt(x2,y2));
                        num++;
                    }
                }
                next_alpha.at(indexAt(x,y)) = (AlphaMapElemT)sum / (AlphaMapElemT)num;
            }
        }
        alpha_map = std::move(next_alpha);
    }

    HeightMapT HeightMapWithAlpha::getHeightMap() {
        return height_map;
    }

    HeightMapElemT HeightMapWithAlpha::getHeightAt(size_t index) const {
        return height_map.at(index);
    }

    AlphaMapElemT HeightMapWithAlpha::getAlphaAt(const size_t index) const {
        return alpha_map.at(index);
    }

    int HeightMapWithAlpha::indexAt(const int x, const int y) const {
        return y * texture_width + x;
    }
}
