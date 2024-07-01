#include <plateau/height_map_generator//heightmap_generator.h>
#include <iostream>
#include <fstream>
#include "png.h"
#include <filesystem>
#include <algorithm>

namespace plateau::heightMapGenerator {

    struct Tile {
        TVec2d Max;
        TVec2d Min;
        int ID;
        std::shared_ptr<std::vector<Triangle>> Triangles;

        Tile(int id, const TVec2d min, const TVec2d max){
            ID = id;
            Min = min;
            Max = max;
            Triangles = std::make_shared<std::vector<Triangle>>();
        }

        void getCornerPoints(TVec2d& p1, TVec2d& p2, TVec2d& p3, TVec2d& p4) const {
            p1 = Min;
            p2 = TVec2d(Min.x, Max.y);
            p3 = TVec2d(Max.x, Min.y);
            p4 = Max;
        }

        // pointが範囲内にあるかどうかを判定する関数
        bool isWithin(const TVec2d& point) const {
            return (point.x >= Min.x && point.x <= Max.x && point.y >= Min.y && point.y <= Max.y);
        }

        bool isAlmostWithin(const TVec2d& point, const double toleranceMargin) const {
            return (point.x >= Min.x - toleranceMargin && point.x <= Max.x + toleranceMargin && point.y >= Min.y - toleranceMargin && point.y <= Max.y + toleranceMargin);
        }
    };

    // MeshからHeightMap画像となる16bitグレースケール配列を生成し、適用範囲となる位置を計算します
    // ENUに変換してから処理を実行し、元のCoordinateに変換して値を返します
    HeightMapT HeightmapGenerator::generateFromMesh(
        const plateau::polygonMesh::Mesh& InMesh, size_t TextureWidth, size_t TextureHeight, const TVec2d& margin,
        const geometry::CoordinateSystem coordinate, bool fillEdges, TVec3d& outMin, TVec3d& outMax, TVec2f& outUVMin, TVec2f& outUVMax) {

        const auto& InVertices = InMesh.getVertices();
        const auto& InIndices = InMesh.getIndices();

        auto triangles = TriangleList::generateFromMesh(InIndices, InVertices, coordinate);
        auto& extent = triangles.Extent;

        //UV
        if (!getUVExtent(InMesh.getUV1(), outUVMin, outUVMax)) {
            //UV情報が取得できなかった場合 0,1に設定
            outUVMin.x = outUVMin.y = 0.f;
            outUVMax.x = outUVMax.y = 1.f;
        }
        else {
            //UV情報が取得できた場合、UVに余白を追加
            if (margin.x != 0 || margin.y != 0) {
                auto uvSize = TVec2f(outUVMax.x - outUVMin.x, outUVMax.y - outUVMin.y);
                auto baseExtentSize = TVec2d(extent.getXLength() / uvSize.x, extent.getYLength() / uvSize.y);
                auto marginPercent = TVec2f(((float)margin.x) / baseExtentSize.x, ((float)margin.y) / baseExtentSize.y);
                outUVMax.x += marginPercent.x;
                outUVMax.y += marginPercent.y;
            }
        }

        //Extentに余白を追加
        extent.Max.x += margin.x;
        extent.Max.y += margin.y;

        // ここでハイトマップを生成します
        auto map_with_alpha = generateFromMeshAndTriangles(InMesh, TextureWidth, TextureHeight, fillEdges, triangles, HeightMapT (0));

        extent.convertCoordinateTo(coordinate);
        outMin = extent.Min;
        outMax = extent.Max;

        return map_with_alpha.getHeightMap();
    }


    // 引数のinitial_height_mapは、特になければ空のvectorを渡し、他のマップと合わせたければそれを指定します。
    HeightMapWithAlpha
    HeightmapGenerator::generateFromMeshAndTriangles(const plateau::polygonMesh::Mesh& in_mesh, size_t texture_width,
                                                     size_t texture_height,
                                                     bool fillEdges, TriangleList& triangles,
                                                     const HeightMapT& initial_height_map) {
        //Tile生成 : Triangleのイテレーションを減らすため、グリッド分割して範囲ごとに処理します
        std::vector<Tile> tiles;
        size_t division = getTileDivision(triangles.Triangles.size()); //縦横のグリッド分割数
        const double xTiles = (double)texture_width / division;
        const double yTiles = (double)texture_height / division;
        int tileIndex = 0;
        TVec2d prev(0, 0);
        auto extent = triangles.Extent;

        for (double y = yTiles; y <= texture_height; y += yTiles) {
            for (double x = xTiles; x <= texture_width; x += xTiles) {

                const TVec2d min = prev;
                const TVec2d max(x, y);
                Tile tile(tileIndex++, min, max);
                prev.x = x;
                if (x + xTiles > texture_width) {
                    tile.Max.x = texture_width;
                    prev.x = 0;
                }
                if (y + yTiles > texture_height)
                    tile.Max.y = texture_height;
                tiles.push_back(tile);

                /**　タイル内に含まれるTriangleをセット
                * 四角タイル内に三角メッシュの頂点が含まれるか
                * 三角メッシュ内に四角タイルの頂点が含まれるか
                * 四角タイル、三角メッシュの各辺が交差するか
                * の3点について検証します
                */
                for (auto tri: triangles.Triangles) {
                    //3次元座標をテクスチャ上の座標に変換してTriangleがTile範囲内にあるかチェック
                    TVec2d tmin(0, 0);
                    TVec2d tmax(texture_width, texture_height);
                    //Triangle頂点(Texture座標）
                    const auto& v1 = getPositionFromPercent(extent.getPercent(TVec2d(tri.V1)), tmin, tmax);
                    const auto& v2 = getPositionFromPercent(extent.getPercent(TVec2d(tri.V2)), tmin, tmax);
                    const auto& v3 = getPositionFromPercent(extent.getPercent(TVec2d(tri.V3)), tmin, tmax);
                    const double tolerance = xTiles / 2;
                    if (tile.isWithin(v1) || tile.isWithin(v2) || tile.isWithin(v3)) {
                        tile.Triangles->push_back(tri);
                        continue;
                    }

                    //Tile頂点(Texture座標）
                    TVec2d r1, r2, r3, r4;
                    tile.getCornerPoints(r1, r2, r3, r4);

                    //Triangleをテクスチャ上の座標に変換してTile頂点がTriangle範囲内にあるかチェック
                    Triangle TexTri{TVec3d(v1.x, v1.y), TVec3d(v2.x, v2.y), TVec3d(v3.x, v3.y)};
                    if (TexTri.isInside(r1) || TexTri.isInside(r2) || TexTri.isInside(r3) || TexTri.isInside(r4)) {
                        tile.Triangles->push_back(tri);
                        continue;
                    }

                    // 3角形と4角形の各辺が交差しているかどうかを判定(Texture座標）
                    if (tri.segmentsIntersect(v1, v2, r1, r2) || tri.segmentsIntersect(v1, v2, r2, r3) ||
                        tri.segmentsIntersect(v1, v2, r3, r4) ||
                        tri.segmentsIntersect(v2, v3, r1, r2) || tri.segmentsIntersect(v2, v3, r2, r3) ||
                        tri.segmentsIntersect(v2, v3, r3, r4) ||
                        tri.segmentsIntersect(v3, v1, r1, r2) || tri.segmentsIntersect(v3, v1, r2, r3) ||
                        tri.segmentsIntersect(v3, v1, r3, r4)) {
                        tile.Triangles->push_back(tri);
                        continue;
                    }
                }
            }
            prev.y = (y < texture_height) ? y : 0;
        }

        const auto TextureDataSize = texture_width * texture_height;

        // Initialize Texture Data Array
        auto map_with_alpha = HeightMapWithAlpha(texture_width, texture_height, extent.getXLength(), extent.getYLength(), initial_height_map);

        size_t index = 0;
        for (int y = (int)texture_height - 1; y >= 0; y--) {
            for (int x = 0; x < texture_width; x++) {

                const auto& percent = TVec2d((double)x / (double)texture_width, (double)y / (double)texture_height);
                const auto& p = getPositionFromPercent(percent, TVec2d(extent.Min), TVec2d(extent.Max));

                HeightMapElemT GrayValue = map_with_alpha.getHeightAt(index); // 初期値
                bool ValueSet = false;
                for (auto& tile : tiles) {
                    if (tile.isWithin(TVec2d(x, y))) {
                        for (auto tri : *tile.Triangles) {
                            if (tri.isInside(p)) {
                                double Height = tri.getHeight(p);
                                double HeightPercent = getHeightToPercent(Height, extent.Min.z, extent.Max.z);
                                GrayValue = getPercentToGrayScale(HeightPercent);
                                ValueSet = true;
                                break;
                            }
                        }
                        if(ValueSet) break;
                    }

                }
                if (index < TextureDataSize) {
                    map_with_alpha.setHeightAt(index, GrayValue);
                    map_with_alpha.setAlphaAt(index, ValueSet ? 1 : 0);
                }

                index++;
            }

        }

        //透明部分をエッジ色でFill
        if(fillEdges)
            map_with_alpha.fillTransparentEdges();

        //平滑化
        map_with_alpha.applyConvolutionFilterForHeightMap();
        return map_with_alpha;
    }

    //Mesh数に応じてTile分割数を決めます
    size_t HeightmapGenerator::getTileDivision(size_t triangleSize) {
        if (triangleSize > 1000000)
            return 18;
        if (triangleSize > 100000)
            return 16;
        if (triangleSize > 10000)
            return 12;
        return 8;
    }

    double HeightmapGenerator::getPositionFromPercent(const double percent, const double min, const double max) {
        const double dist = abs(max - min);
        const auto pos = min + (dist * percent);
        const auto clamped = std::clamp(pos, min, max);
        return clamped;
    }

    TVec2d HeightmapGenerator::getPositionFromPercent(const TVec2d& percent, const TVec2d& min, const TVec2d& max) {
        return TVec2d(getPositionFromPercent(percent.x, min.x, max.x), getPositionFromPercent(percent.y, min.y, max.y));
    }

    double HeightmapGenerator::getHeightToPercent(const double height, const double min, const double max) {
        const auto height_clamped = std::clamp(height, min, max);
        const double dist = abs(max - min);
        const double height_in_dist = abs(height_clamped - min);
        const auto percent = height_in_dist / dist;
        return percent;
    }

    HeightMapElemT HeightmapGenerator::getPercentToGrayScale(const double percent) {
        const auto value = static_cast<double>(HeightMapNumericMax) * percent;
        const auto clamped = std::clamp(value, 0.0, static_cast<double>(HeightMapNumericMax));
        return static_cast<HeightMapElemT>(clamped);
    }

    TVec3d HeightmapGenerator::convertCoordinateFrom(const geometry::CoordinateSystem coordinate, const TVec3d vertice) {
        return geometry::GeoReference::convertAxisToENU(coordinate, vertice);
    }

    //UVの最大、最小値を取得します。値が取得できなかった場合はfalseを返します。
    bool HeightmapGenerator::getUVExtent(plateau::polygonMesh::UV uvs, TVec2f& outMin, TVec2f& outMax) {
        TVec2f Min, Max;
        for (auto uv : uvs) {
            if (Max.x == 0) Max.x = uv.x;
            if (Min.x == 0) Min.x = uv.x;
            Max.x = std::max(Max.x, uv.x);
            Min.x = std::min(Min.x, uv.x);

            if (Max.y == 0) Max.y = uv.y;
            if (Min.y == 0) Min.y = uv.y;
            Max.y = std::max(Max.y, uv.y);
            Min.y = std::min(Min.y, uv.y);
        }
        outMin = Min;
        outMax = Max;
        return !(outMin.x == 0.f && outMin.y == 0.f && outMax.x == 0.f && outMax.y == 0.f);
    }


    // 16bitグレースケールのpng画像を保存します
    void HeightmapGenerator::savePngFile(const std::string& file_path, size_t width, size_t height, HeightMapElemT* data) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
        FILE* fp;
        _wfopen_s(&fp, regular_name.c_str(), L"wb");
#else
        FILE* fp = fopen(file_path.c_str(), "wb");
#endif       
        if (!fp) {
            throw std::runtime_error("Error: Failed to open PNG file for writing.");
        }

        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
            fclose(fp);
            throw std::runtime_error("Error: png_create_write_struct failed.");
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            throw std::runtime_error("Error: png_create_info_struct failed.");
        }
        png_init_io(png_ptr, fp);
        png_set_IHDR(png_ptr, info_ptr, width, height,
            16, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        png_write_info(png_ptr, info_ptr);

        png_bytep row = (png_bytep)malloc(width * sizeof(png_uint_16));
        if (!row) {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            throw std::runtime_error("Error: Failed to allocate memory for PNG row.");
        }

        int index = 0;
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                png_uint_16 value = (png_uint_16)data[index];
                index++;
                row[x * 2] = (png_byte)(value >> 8);
                row[x * 2 + 1] = (png_byte)(value & 0xFF);
            }
            png_write_row(png_ptr, row);
        }

        free(row);
        png_write_end(png_ptr, NULL);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
    }

    // PNG画像を読み込み、グレースケールの配列を返します
    HeightMapT HeightmapGenerator::readPngFile(const std::string& file_path, size_t width, size_t height) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
        FILE* fp;
        _wfopen_s(&fp, regular_name.c_str(), L"rb");
#else
        FILE* fp = fopen(file_path.c_str(), "rb");
#endif
        if (!fp) {
            throw std::runtime_error("Error: Unable to open file for reading.");
        }

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
            fclose(fp);
            throw std::runtime_error("Error: Failed to create PNG read struct.");
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            fclose(fp);
            throw std::runtime_error("Error: Failed to create PNG info struct.");
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 0);
        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        int color_type = png_get_color_type(png_ptr, info_ptr);

        if (bit_depth != 16 || color_type != PNG_COLOR_TYPE_GRAY) {
            throw std::runtime_error("Error: Invalid PNG format. Expected 16-bit grayscale.");
        }

        HeightMapT grayscaleData(width * height);

        png_bytepp row_pointers = (png_bytepp)malloc(sizeof(png_bytep) * height);
        for (size_t y = 0; y < height; ++y) {
            row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
        }

        png_read_image(png_ptr, row_pointers);

        for (size_t y = 0; y < height; ++y) {
            png_bytep row = row_pointers[y];
            for (int x = 0; x < width; ++x) {
                grayscaleData[y * width + x] = (row[x * 2] << 8) + row[x * 2 + 1];
            }
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);

        return grayscaleData;
    }

    // 16bitグレースケールのpng画像を保存します   
    void HeightmapGenerator::saveRawFile(const std::string& file_path, size_t width, size_t height, HeightMapElemT* data) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
#else
        const auto regular_name = std::filesystem::u8path(file_path).u8string();
#endif

        std::ofstream outputFile(regular_name, std::ios::out | std::ios::binary);

        if (!outputFile) {
            throw std::runtime_error("Error: Unable to open file for writing.");
            return;
        }

        // Write image data
        for (int i = 0; i < width * height; ++i) {
            HeightMapElemT pixelValue = data[i];
            outputFile.write(reinterpret_cast<const char*>(&pixelValue), sizeof(HeightMapElemT));
        }

        outputFile.close();
    }

    // Raw画像を読み込み、グレースケールの配列を返します
    HeightMapT HeightmapGenerator::readRawFile(const std::string& file_path, size_t width, size_t height) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
#else
        const auto regular_name = std::filesystem::u8path(file_path).u8string();
#endif

        std::ifstream file(regular_name, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error: Unable to open file for reading. ");
        }

        HeightMapT grayscaleData(width * height);
        // データをバイナリとして読み込む
        file.read(reinterpret_cast<char*>(&grayscaleData[0]), width * height * sizeof(HeightMapElemT));
        file.close();

        return grayscaleData;
    }
} // namespace texture

