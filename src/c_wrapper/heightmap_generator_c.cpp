#include <plateau/height_map_generator//heightmap_generator.h>
#include "libplateau_c.h"
#include <plateau/polygon_mesh/mesh.h>
#include <cstring>

using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::heightMapGenerator;
using namespace plateau::geometry;

extern "C" {
    // 注意 : out_heightmap_dataはコピー終了後にrelease_heightmap_dataを呼んで削除する必要あり
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_generator_generate_from_mesh(
            Mesh* const src_mesh,
            const size_t TextureWidth,
            const size_t TextureHeight,
            const TVec3d margin,
            const CoordinateSystem coordinate,
            const bool fillEdges,
            const bool applyConvolutionFilterForHeightMap,
            TVec3d* outMin,
            TVec3d* outMax,
            TVec2f* outUVMin,
            TVec2f* outUVMax,
            const HeightMapElemT** out_heightmap_data,
            size_t* dataSize
    ) {
        API_TRY{
            HeightmapGenerator generator;
            const auto& vec = generator.generateFromMesh(
                    *src_mesh, TextureWidth, TextureHeight, TVec2d(margin.x, margin.y) , coordinate,
                    fillEdges, applyConvolutionFilterForHeightMap,
                    *outMin, *outMax, *outUVMin, *outUVMax);
            HeightMapElemT* heightmap_data = new HeightMapElemT [vec.size()];
            memcpy(heightmap_data, vec.data(), sizeof(HeightMapElemT) * vec.size());
            *out_heightmap_data = heightmap_data;
            *dataSize = vec.size();
            return APIResult::Success;
        } 
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_save_png_file(
            const char* filename, const size_t width, const size_t height, HeightMapElemT* data
    ) {
        API_TRY{
            HeightmapGenerator::savePngFile(std::string(filename), width, height, data);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    // 注意 : out_heightmap_dataはコピー終了後にrelease_heightmap_dataを呼んで削除する必要あり
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_read_png_file(
      const char* filename, const size_t width, const size_t height, const HeightMapElemT** out_heightmap_data, size_t* dataSize
    ) {
        API_TRY{
            const auto& vec = HeightmapGenerator::readPngFile(std::string(filename), width, height);
            HeightMapElemT* heightmap_data = new HeightMapElemT[vec.size()];
            memcpy(heightmap_data, vec.data(), sizeof(HeightMapElemT) * vec.size());
            *out_heightmap_data = heightmap_data;
            *dataSize = vec.size();
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_save_raw_file(
        const char* filename, const size_t width, const size_t height, HeightMapElemT* data
    ) {
        API_TRY{
            HeightmapGenerator::saveRawFile(std::string(filename), width, height, data);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    // 注意 : out_heightmap_dataはコピー終了後にrelease_heightmap_dataを呼んで削除する必要あり
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API heightmap_read_raw_file(
    const char* filename, const size_t width, const size_t height, const HeightMapElemT** out_heightmap_data, size_t* dataSize
    ) {
        API_TRY{
            const auto & vec = HeightmapGenerator::readRawFile(std::string(filename), width, height);
            HeightMapElemT* heightmap_data = new HeightMapElemT[vec.size()];
            memcpy(heightmap_data, vec.data(), sizeof(HeightMapElemT) * vec.size());
            *out_heightmap_data = heightmap_data;
            *dataSize = vec.size();
            return APIResult::Success;
            } API_CATCH;
            return APIResult::ErrorUnknown;
    }

    // heightmap_data配列削除
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API  release_heightmap_data(uint16_t* heightmap_data) {
        API_TRY{
            delete[] heightmap_data;
        return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

}
