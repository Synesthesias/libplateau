#pragma once

#include "plateau/polygon_mesh/mesh.h"

namespace plateau::texture {
    /**
     * 
     */

    class HeightmapGenerator  {
    public:

        std::vector<uint16_t> generateFromMesh(const plateau::polygonMesh::Mesh& InMesh, size_t TextureWidth, size_t TextureHeight, TVec2d margin, geometry::CoordinateSystem coordinate, TVec3d& outMin, TVec3d& outMax);

        static void savePngFile(const char* filename, size_t width, size_t height, uint16_t* data);
        static std::vector<uint16_t> readPngFile(const char* filename, size_t width, size_t height);
        static void saveRawFile(const char* filename, size_t width, size_t height, uint16_t* data);
        static std::vector<uint16_t> readRawFile(const char* filename, size_t width, size_t height);

    private:
        double getPositionFromPercent(double percent, double min, double max);
        double getHeightToPercent(double height, double min, double max);
        uint16_t getPercentToGrayScale(double percent);
        TVec3d convertCoordinateFrom(geometry::CoordinateSystem coordinate, TVec3d vertice);
    };

} // namespace plateau::texture
