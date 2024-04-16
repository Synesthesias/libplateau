#include <plateau/texture/heightmap_generator.h>
#include "libplateau_c.h"
#include <plateau/polygon_mesh/mesh.h>

using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::texture;
using namespace plateau::geometry;

extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_generator_generate_from_mesh(
                    Mesh* const src_mesh,
                    size_t TextureWidth,
                    size_t TextureHeight,
                    TVec2d margin,
                    CoordinateSystem coordinate,
                    TVec3d* outMin,
                    TVec3d* outMax,
                    const uint16_t** const out_heightmap_data,
                    size_t* dataSize
    ) {
        API_TRY{
            HeightmapGenerator generator;
            auto vec = generator.generateFromMesh(*src_mesh, TextureWidth, TextureHeight, margin, coordinate, *outMin, *outMax);
            *dataSize = vec.size();
            *out_heightmap_data = vec.data();
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_generator_generate_from_mesh2(HeightmapGenerator* handle, Mesh* const src_mesh, size_t TextureWidth, size_t TextureHeight, 
        TVec2d margin, CoordinateSystem coordinate,
        TVec3d* outMin, TVec3d* outMax, const uint16_t** const out_heightmap_data, size_t* dataSize) {
        API_TRY{
            auto vec = handle->generateFromMesh(*src_mesh, TextureWidth, TextureHeight, margin, coordinate, *outMin, *outMax);
            *dataSize = vec.size();
            *out_heightmap_data = vec.data();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_save_png_file(
                const char* filename, size_t width, size_t height, uint16_t* data
    ) {
        API_TRY{
            HeightmapGenerator::savePngFile(filename, width, height, data);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_read_png_file(
            const char* filename, size_t width, size_t height, const uint16_t** const out_heightmap_data, size_t* dataSize
    ) {
        API_TRY{
            auto vec = HeightmapGenerator::readPngFile(filename, width, height);
            *dataSize = vec.size();
            *out_heightmap_data = vec.data();
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_save_raw_file(
            const char* filename, size_t width, size_t height, uint16_t* data
    ) {
        API_TRY{
            HeightmapGenerator::saveRawFile(filename, width, height, data);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_read_raw_file(
        const char* filename, size_t width, size_t height, const uint16_t** const out_heightmap_data, size_t* dataSize
    ) {
        API_TRY{
            auto vec = HeightmapGenerator::readRawFile(filename, width, height);
            *dataSize = vec.size();
            *out_heightmap_data = vec.data();
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }


}
