#include <plateau/polygon_mesh/map_attacher.h>
#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/texture/texture_image_base.h>
#include <plateau/texture/texture_packer.h>
#include <cfloat>
#include <cassert>

namespace plateau::polygonMesh {
    namespace fs = std::filesystem;
    using namespace plateau::texture;

    namespace {


        /// 地図タイルを貼り付ける準備として、メッシュにUVを設定します。
        /// メッシュの頂点のうち、もっとも南西にある箇所をUV(0,0)とします。
        /// メッシュを包む正方形の中でもっとも北東の箇所をUV(1,1)とします。
        /// メッシュが正方形の場合、メッシュの北東端は UV(1,1)です。
        /// メッシュが東西方向に長い場合、メッシュの北東端は正方形の右の辺上にあるので UV(1,y) (0<y<1) です。
        /// メッシュが南北方向に長い場合、メッシュの北東端は正方形の上の辺上にあるので UV(x,1) (0<x<1) です。
        /// 頂点の高さは考慮しません。
        void setUVForMap(Mesh& mesh, const TVec3d min_arg, const TVec3d max_arg, const GeoReference& geo_ref) { // NOLINT(performance-unnecessary-value-param)
            auto& vertices = mesh.getVertices();
            auto vertices_count = vertices.size();
            auto uv = std::vector<TVec2f>();
            uv.reserve(vertices_count);
            const auto min = geo_ref.convertAxisToENU(min_arg);
            const auto max = geo_ref.convertAxisToENU(max_arg);
            const auto diff = max - min;
            auto size2d = TVec2d(diff.x, diff.y);
            auto size_longer = std::max(size2d.x, size2d.y); // 縦と横のうち長い方の長さを採用します。非正方形の地形でテクスチャが引き伸ばされることを防ぎます。
            for(int i=0; i<vertices_count; i++) {
                const auto v_with_src_axis = vertices.at(i);
                const auto v = GeoReference::convertAxisToENU(geo_ref.getCoordinateSystem(), v_with_src_axis);
                auto uv_x = (float)((v.x - min.x) / size_longer);
                auto uv_y = (float)((v.y - min.y) / size_longer);
                uv.emplace_back(uv_x, uv_y);
            }
            mesh.setUV1(std::move(uv));
        }

        Extent getExtent(const TVec3d min, const TVec3d max, const GeoReference& geo_reference) { // NOLINT(performance-unnecessary-value-param)
            const auto geo_min = geo_reference.unproject(min);
            const auto geo_max = geo_reference.unproject(max);
            return {
                {geo_min.latitude, geo_min.longitude, -99999},
                { geo_max.latitude, geo_max.longitude, 99999}
            };
        }

    }

    void MapAttacher::attach(Model& model, const std::string& map_url_template, const std::filesystem::path& map_download_dest, const int zoom_level, const GeoReference& geo_reference) {
        auto meshes = model.getAllMeshes();
        for(int i=0; i<meshes.size(); i++) {
            auto mesh = meshes.at(i);
            if(mesh->getVertices().empty()) continue;

            // UVを貼ります。
            const auto [min, max] = mesh->calcBoundingBox();
            setUVForMap(*mesh, min, max, geo_reference);

            // 地図タイルをダウンロードします。
            auto extent = getExtent(min, max, geo_reference);
            auto downloader = VectorTileDownloader(map_download_dest.u8string(), extent, zoom_level);
            downloader.setUrl(map_url_template);
            auto tiles = downloader.downloadAll();

            if(!tiles.anyTileSucceed()) continue;

            // タイル画像の大きさを求めます。
            size_t tile_image_width = 0;
            size_t tile_image_height = 0;
            for(const auto& tile : tiles.tiles()) {
                if(tile.result != HttpResult::Success) continue;
                bool image_loaded = false;
                auto image = TextureImageBase::tryCreateFromFile(tile.image_path, 9999, image_loaded);
                if(!image_loaded) continue;
                tile_image_width = image->getWidth();
                tile_image_height = image->getHeight();
                // 大きさはすべて同じと仮定するので、1つ確認すれば十分とします。
                break;
            }
            auto min_col = tiles.minColumn();
            auto max_col = tiles.maxColumn();
            auto min_row = tiles.minRow();
            auto max_row = tiles.maxRow();
            assert(min_col <= max_col);
            assert(min_row <= max_row);
            size_t combined_image_width = tile_image_width * (max_col - min_col + 1);
            size_t combined_image_height = tile_image_height * (max_row - min_row + 1);

            // タイルを結合します。
            auto packer = TexturePacker(combined_image_width, combined_image_height);
            auto combined_tile_dir = fs::u8path(tiles.firstSucceed().image_path)
                    .remove_filename()
                    .parent_path()
                    .parent_path()
                    .parent_path();
            std::string combined_file_name = "combined_map_for_mesh_" + std::to_string(i) + "_";
            packer.setSaveFilePath(combined_tile_dir, combined_file_name);
            for (auto r = min_row; r <= max_row; r++) {
                for (auto c = min_col; c <= max_col; c++) {
                    const auto& tile = tiles.getTile(c, r);
                    bool image_loaded = false;
                    auto image = TextureImageBase::tryCreateFromFile(tile.image_path, 9999, image_loaded);
                    int pack_canvas_id = -1;
                    if (!image_loaded) {
                        auto blank_image = TextureImageBase::createNewTexture(tile_image_width, tile_image_height);
                        packer.packImage(blank_image.get(),
                                         "dummy_path_blank_image " + std::to_string(c) + std::to_string(r),
                                         pack_canvas_id);
                        continue;
                    }
                    packer.packImage(image.get(), tile.image_path, pack_canvas_id);
                }
            }
            packer.flush();
        }
    }
}
