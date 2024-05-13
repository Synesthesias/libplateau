#include <plateau/texture/heightmap_generator.h>
#include <plateau/geometry/geo_reference.h>
#include <iostream>
#include <fstream>
#include "png.h"
#include <filesystem>

namespace plateau::texture {

    struct HeightMapExtent {
        TVec3d Max;
        TVec3d Min;

        void setVertice(TVec3d vertice) {
            if (Max.x == 0) Max.x = vertice.x;
            if (Min.x == 0) Min.x = vertice.x;
            Max.x = std::max(Max.x, vertice.x);
            Min.x = std::min(Min.x, vertice.x);

            if (Max.y == 0) Max.y = vertice.y;
            if (Min.y == 0) Min.y = vertice.y;
            Max.y = std::max(Max.y, vertice.y);
            Min.y = std::min(Min.y, vertice.y);

            if (Max.z == 0) Max.z = vertice.z;
            if (Min.z == 0) Min.z = vertice.z;
            Max.z = std::max(Max.z, vertice.z);
            Min.z = std::min(Min.z, vertice.z);
        }

        double getXLength() {
            return std::abs(Max.x - Min.x);
        }

        double getYLength() {
            return std::abs(Max.y - Min.y);
        }

        void convertCoordinateFrom(geometry::CoordinateSystem coordinate) {
            Max = geometry::GeoReference::convertAxisToENU(coordinate, Max);
            Min = geometry::GeoReference::convertAxisToENU(coordinate, Min);
            normalizeDirection(coordinate);
        }

        void convertCoordinateTo(geometry::CoordinateSystem coordinate) {
            Max = geometry::GeoReference::convertAxisFromENUTo(coordinate, Max);
            Min = geometry::GeoReference::convertAxisFromENUTo(coordinate, Min); 
            normalizeDirection(coordinate);
        }

        void normalizeDirection(geometry::CoordinateSystem coordinate) {
            TVec3d newMin = Min;
            TVec3d newMax = Max;    
            if (coordinate == geometry::CoordinateSystem::EUN) { 
                //Unity
                newMin = TVec3d(Min.x, Min.y, Min.z);
                newMax = TVec3d(Max.x, Max.y, Max.z);
            } else if (coordinate == geometry::CoordinateSystem::ESU) {
                //Unreal
                newMin = TVec3d(Min.x, Max.y, Min.z);
                newMax = TVec3d(Max.x, Min.y, Max.z);              
            }
            Min = newMin;
            Max = newMax;
        }
    };

    struct Triangle {

        TVec3d V1;
        TVec3d V2;
        TVec3d V3;
        int id;

        // 2点間のベクトルを計算する関数
        TVec3d vectorBetweenPoints(const TVec3d& p1, const TVec3d& p2) {
            return { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
        }

        // 2つのベクトルの外積を計算する関数
        TVec3d crossProduct(const TVec3d& v1, const TVec3d& v2) {
            return { v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x };
        }

        // 平面の方程式の係数を計算する関数
        void planeEquationCoefficients(const TVec3d& p1, const TVec3d& p2, const TVec3d& p3, double& A, double& B, double& C, double& D) {
            TVec3d vec1 = vectorBetweenPoints(p1, p2);
            TVec3d vec2 = vectorBetweenPoints(p1, p3);
            TVec3d normal = crossProduct(vec1, vec2);
            A = normal.x;
            B = normal.y;
            C = normal.z;
            D = -(A * p1.x + B * p1.y + C * p1.z);
        }

        // 指定された x, y 座標から z 座標を計算する関数
        double getHeight(double x, double y) {
            double A, B, C, D;
            planeEquationCoefficients(V1, V2, V3, A, B, C, D);
            return (-D - A * x - B * y) / C;
        }

        bool isInside(double x, double y) {
            return isInside(TVec2d(V1.x, V1.y), TVec2d(V2.x, V2.y), TVec2d(V3.x, V3.y), TVec2d(x, y));
        }

        double crossProduct2D(const TVec2d& A, const TVec2d& B, const TVec2d& C) {
            return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
        }

        // 点Pが三角形ABCの内側にあるかどうかを判定する関数
        bool isInside(const TVec2d& A, const TVec2d& B, const TVec2d& C, const TVec2d& P) {
            double crossABP = crossProduct2D(A, B, P);
            double crossBCP = crossProduct2D(B, C, P);
            double crossCAP = crossProduct2D(C, A, P);

            // 点Pが３角形ABCの内側にあるかどうかを判定
            if ((crossABP >= 0 && crossBCP >= 0 && crossCAP >= 0) ||
                (crossABP <= 0 && crossBCP <= 0 && crossCAP <= 0)) {
                return true;
            }
            return false;
        }
    };

    // MeshからHeightMap画像となる16bitグレースケール配列を生成し、適用範囲となる位置を計算します
    // ENUに変換してから処理を実行し、元のCoordinateに変換して値を返します
    std::vector<uint16_t> HeightmapGenerator::generateFromMesh(
        const plateau::polygonMesh::Mesh& InMesh, size_t TextureWidth, size_t TextureHeight, TVec2d margin, 
        geometry::CoordinateSystem coordinate, TVec3d& outMin, TVec3d& outMax, TVec2f& outUVMin, TVec2f& outUVMax) {

        const auto& InVertices = InMesh.getVertices();
        const auto& InIndices = InMesh.getIndices();

        HeightMapExtent extent;
        std::vector<Triangle> triangles;

        for (size_t i = 0; i < InIndices.size(); i += 3) {

            Triangle tri;
            tri.V1 = convertCoordinateFrom(coordinate, InVertices.at(InIndices[i]));
            tri.V2 = convertCoordinateFrom(coordinate, InVertices.at(InIndices[i + 1]));
            tri.V3 = convertCoordinateFrom(coordinate, InVertices.at(InIndices[i + 2]));
            extent.setVertice(tri.V1);
            extent.setVertice(tri.V2);
            extent.setVertice(tri.V3);
            triangles.push_back(tri);
        }

        //UV
        if (!getUVExtent(InMesh.getUV1(), outUVMin, outUVMax)) {
            //UV情報が取得できなかった場合 0,1に設定
            outUVMin.x = outUVMin.y = 0.f;
            outUVMax.x = outUVMax.y = 1.f;
        } else {
            //UV情報が取得できた場合、UVに余白を追加
            if (margin.x != 0 || margin.y != 0) {
                auto uvSize = TVec2f(outUVMax.x - outUVMin.x, outUVMax.y - outUVMin.y);
                auto baseExtentSize = TVec2d(extent.getXLength() / uvSize.x, extent.getYLength() / uvSize.y);
                auto marginPercent = TVec2f(margin.x / baseExtentSize.x, margin.y / baseExtentSize.y);
                outUVMax.x += marginPercent.x;
                outUVMax.y += marginPercent.y;
            }
        }

        //余白を追加
        extent.Max.x += margin.x;
        extent.Max.y += margin.y;

        int TextureDataSize = TextureWidth * TextureHeight;

        // Initialize Texture Data Array
        uint16_t* TextureData = new uint16_t[TextureDataSize];

        size_t index = 0;
        for (int y = TextureHeight - 1; y >= 0; y--) {
            for (int x = 0; x < TextureWidth; x++) {

                double xpercent = (double)x / (double)TextureWidth;
                double ypercent = (double)y / (double)TextureHeight;

                double PosX = getPositionFromPercent(xpercent, extent.Min.x, extent.Max.x);
                double PosY = getPositionFromPercent(ypercent, extent.Min.y, extent.Max.y);

                uint16_t GrayValue = 0;
                double Height = 0;
                double HeightPercent = 0;

                for (auto tri : triangles) {
                    if (tri.isInside(PosX, PosY)) {
                        Height = tri.getHeight(PosX, PosY);
                        HeightPercent = getHeightToPercent(Height, extent.Min.z, extent.Max.z);
                        GrayValue = getPercentToGrayScale(HeightPercent);
                        break;
                    }
                }

                if(index < TextureDataSize)
                    TextureData[index] = GrayValue;
                index++;
            }
        }

        //平滑化
        applyConvolutionFilter(TextureData, TextureWidth, TextureHeight);

        std::vector<uint16_t> heightMapData(TextureWidth * TextureHeight);
        memcpy(heightMapData.data(), TextureData, sizeof(uint16_t) * TextureDataSize);

        delete[] TextureData;
        extent.convertCoordinateTo(coordinate);
        outMin = extent.Min;
        outMax = extent.Max;
        return heightMapData;
    }

    double HeightmapGenerator::getPositionFromPercent(double percent, double min, double max) {
        double dist = abs(max - min);
        return min + (dist * percent);
    }

    double HeightmapGenerator::getHeightToPercent(double height, double min, double max) {
        double dist = abs(max - min);
        double height_in_dist = abs(height - min);
        return height_in_dist / dist;
    }

    uint16_t HeightmapGenerator::getPercentToGrayScale(double percent) {
        uint16_t size = 65535;
        return static_cast<uint16_t>(static_cast<double>(size) * percent);
    }

    TVec3d HeightmapGenerator::convertCoordinateFrom(geometry::CoordinateSystem coordinate, TVec3d vertice) {
        return geometry::GeoReference::convertAxisToENU(coordinate, vertice);
    }

    void HeightmapGenerator::applyConvolutionFilter(uint16_t* image, const size_t width, const size_t height) {
        size_t imageSize = width * height;
        uint16_t* tempImage = new uint16_t[imageSize];
        memcpy(tempImage, image, sizeof(uint16_t) * imageSize);
        for (size_t y = 1; y < height - 1; ++y) {
            for (size_t x = 1; x < width - 1; ++x) {
                // 3x3の領域のピクセル値の平均を計算
                int sum = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        sum += image[(y + dy) * width + (x + dx)];
                    }
                }
                tempImage[y * width + x] = sum / 9; 
            }
        }
        memcpy(image, tempImage, sizeof(uint16_t) * imageSize);
        delete[] tempImage;
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
    void HeightmapGenerator::savePngFile(const std::string& file_path, size_t width, size_t height, uint16_t* data) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
        FILE* fp = _wfopen(regular_name.c_str(), L"wb");
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
    std::vector<uint16_t> HeightmapGenerator::readPngFile(const std::string& file_path, size_t width, size_t height) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
        FILE* fp = _wfopen(regular_name.c_str(), L"rb");
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

        std::vector<uint16_t> grayscaleData(width * height);

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
    void HeightmapGenerator::saveRawFile(const std::string& file_path, size_t width, size_t height, uint16_t* data) {

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
            uint16_t pixelValue = data[i];
            outputFile.write(reinterpret_cast<const char*>(&pixelValue), sizeof(uint16_t));
        }

        outputFile.close();
    }

    // Raw画像を読み込み、グレースケールの配列を返します
    std::vector<uint16_t> HeightmapGenerator::readRawFile(const std::string& file_path, size_t width, size_t height) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_path).wstring();
#else
        const auto regular_name = std::filesystem::u8path(file_path).u8string();
#endif

        std::ifstream file(regular_name, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error: Unable to open file for reading. ");
        }

        std::vector<uint16_t> grayscaleData(width * height);
        // データをバイナリとして読み込む
        file.read(reinterpret_cast<char*>(&grayscaleData[0]), width * height * sizeof(uint16_t));
        file.close();

        return grayscaleData;
    }
} // namespace texture

