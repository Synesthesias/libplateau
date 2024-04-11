#include <plateau/texture/heightmap_generator.h>
#include <iostream>
#include <fstream>
#include "png.h"

namespace plateau::texture {

    std::vector<uint16_t> HeightmapGenerator::generateFromMesh(
        const plateau::polygonMesh::Mesh& InMesh, size_t TextureWidth, size_t TextureHeight, TVec3d& outMin, TVec3d& outMax) {

        const auto& InVertices = InMesh.getVertices();
        const auto& InIndices = InMesh.getIndices();

        double MaxX = 0, MinX = 0;
        double MaxY = 0, MinY = 0;
        double MaxZ = 0, MinZ = 0;

        const auto FaceCount = InIndices.size() / 3;
        for (const auto& Vertex : InVertices) {

            if (MaxX == 0) MaxX = Vertex.x;
            if (MinX == 0) MinX = Vertex.x;
            MaxX = std::max(MaxX, Vertex.x);
            MinX = std::min(MinX, Vertex.x);

            if (MaxY == 0) MaxY = Vertex.y;
            if (MinY == 0) MinY = Vertex.y;
            MaxY = std::max(MaxY, Vertex.y);
            MinY = std::min(MinY, Vertex.y);

            if (MaxZ == 0) MaxZ = Vertex.z;
            if (MinZ == 0) MinZ = Vertex.z;
            MaxZ = std::max(MaxZ, Vertex.z);
            MinZ = std::min(MinZ, Vertex.z);
        }

        //余白を追加
        MaxX += 500;
        MaxY += 500;

        TVec3d ExtMax = TVec3d(MaxX, MaxY, MaxZ);
        TVec3d ExtMin = TVec3d(MinX, MinY, MinZ);

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

        std::vector<Triangle> triangles;

        for (size_t i = 0; i < InIndices.size(); i += 3) {

            Triangle tri;
            tri.V1 = InVertices.at(InIndices[i]);
            tri.V2 = InVertices.at(InIndices[i + 1]);
            tri.V3 = InVertices.at(InIndices[i + 2]);
            triangles.push_back(tri);
        }

        int TotalPixels = TextureWidth * TextureHeight;
        int TextureDataSize = TotalPixels;

        // Initialize Texture Data Array
        uint16_t* TextureData = new uint16_t[TextureDataSize];

        size_t index = 0;
        for (int y = 0; y < TextureHeight; y++) {
            for (int x = 0; x < TextureWidth; x++) {

                double xpercent = (double)x / (double)TextureWidth;
                double ypercent = (double)y / (double)TextureHeight;

                double PosX = getPositionFromPercent(xpercent, MinX, MaxX);
                double PosY = getPositionFromPercent(ypercent, MinY, MaxY);

                uint16_t GrayValue = 0;
                double Height = 0;
                double HeightPercent = 0;

                for (auto tri : triangles) {
                    if (tri.isInside(PosX, PosY)) {
                        Height = tri.getHeight(PosX, PosY);
                        HeightPercent = getHeightToPercent(Height, MinZ, MaxZ);
                        GrayValue = getPercentToGrayScale(HeightPercent);
                        break;
                    }
                }
                TextureData[index] = GrayValue;
                index++;
            }
        }
        /*
        FString SavePath = FPaths::ConvertRelativePathToFull(*(FPaths::ProjectContentDir() + FString("PLATEAU/"))) + FString("HeightMap.png");
        UE_LOG(LogTemp, Error, TEXT("Save png %s"), *SavePath);
        savePngFile(TCHAR_TO_ANSI(*SavePath), TextureWidth, TextureHeight, TextureData);

        FString RawSavePath = FPaths::ConvertRelativePathToFull(*(FPaths::ProjectContentDir() + FString("PLATEAU/"))) + FString("HeightMap.raw");
        UE_LOG(LogTemp, Error, TEXT("Save raw %s"), *RawSavePath);
        saveRawFile(TCHAR_TO_ANSI(*RawSavePath), TextureWidth, TextureHeight, TextureData);
        */
        std::vector<uint16_t> heightMapData(TextureWidth * TextureHeight);
        std::memcpy(heightMapData.data(), TextureData, sizeof(uint16_t) * TextureWidth * TextureHeight);

        delete[] TextureData;

        outMin = ExtMin;
        outMax = ExtMax;
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

    void HeightmapGenerator::savePngFile(const char* filename, size_t width, size_t height, uint16_t* data) {
        FILE* fp = fopen(filename, "wb");
        if (!fp) {
            fprintf(stderr, "Error: Failed to open PNG file for writing\n");
            exit(1);
        }

        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
            fclose(fp);
            fprintf(stderr, "Error: png_create_write_struct failed\n");
            exit(1);
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            fprintf(stderr, "Error: png_create_info_struct failed\n");
            exit(1);
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
            fprintf(stderr, "Error: Failed to allocate memory for PNG row\n");
            exit(1);
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

    // PNG画像を読み込み、グレースケールの配列を取得する関数
    std::vector<uint16_t> HeightmapGenerator::readPngFile(const char* filename, size_t width, size_t height) {
        FILE* fp = fopen(filename, "rb");
        if (!fp) {
            std::cerr << "Error: Unable to open file for reading.\n";
            exit(EXIT_FAILURE);
        }

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) {
            fclose(fp);
            std::cerr << "Error: Failed to create PNG read struct.\n";
            exit(EXIT_FAILURE);
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            fclose(fp);
            std::cerr << "Error: Failed to create PNG info struct.\n";
            exit(EXIT_FAILURE);
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 0);
        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        int color_type = png_get_color_type(png_ptr, info_ptr);

        if (bit_depth != 16 || color_type != PNG_COLOR_TYPE_GRAY) {
            std::cerr << "Error: Invalid PNG format. Expected 16-bit grayscale.\n";
            exit(EXIT_FAILURE);
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

    void HeightmapGenerator::saveRawFile(const char* filename, size_t width, size_t height, uint16_t* data) {

        std::ofstream outputFile(filename, std::ios::out | std::ios::binary);

        if (!outputFile) {
            std::cerr << "Error: Unable to open file for writing.\n";
            return;
        }

        // Write image data
        for (int i = 0; i < width * height; ++i) {
            uint16_t pixelValue = data[i];
            outputFile.write(reinterpret_cast<const char*>(&pixelValue), sizeof(uint16_t));
        }

        outputFile.close();
    }

    std::vector<uint16_t> HeightmapGenerator::readRawFile(const char* filename, size_t width, size_t height) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Unable to open file for reading.\n";
            exit(EXIT_FAILURE);
        }

        std::vector<uint16_t> grayscaleData(width * height);

        // データをバイナリとして読み込む
        file.read(reinterpret_cast<char*>(&grayscaleData[0]), width * height * sizeof(uint16_t));

        file.close();

        return grayscaleData;
    }


} // namespace jpeg

