#include <plateau/polygon_mesh/map_attacher.h>
#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/texture/texture_image_base.h>
#include <plateau/texture/texture_packer.h>
#include <cassert>

namespace plateau::polygonMesh {
    namespace fs = std::filesystem;
    using namespace plateau::texture;

    namespace {


        /// 地図タイルを貼り付ける準備として、メッシュにUVを設定します。
        /// まず、UVを0を最小、1を最大と仮定して計算します。
        /// 頂点の高さは考慮しません。
        /// 次に、UVの最小から最大の範囲について、(0,0)から(1,1)の範囲を変換してuv_minからuv_maxの範囲に投影します。
        void setUVForMap(Mesh& mesh, const TVec3d bounding_box_min_arg, const TVec3d bounding_box_max_arg, const GeoReference& geo_ref, const TVec2f uv_min, const TVec2f uv_max) { // NOLINT(performance-unnecessary-value-param)
            const auto& vertices = mesh.getVertices();
            const auto vertices_count = vertices.size();
            auto uv = std::vector<TVec2f>();
            uv.reserve(vertices_count);
            auto min = geo_ref.convertAxisToENU(bounding_box_min_arg);
            auto max = geo_ref.convertAxisToENU(bounding_box_max_arg);

            // 座標変換後に大小関係逆転することがあるため修正
            const auto tmp = min;
            min.x = std::min(tmp.x, max.x);
            min.y = std::min(tmp.y, max.y);
            min.z = std::min(tmp.z, max.z);
            max.x = std::max(tmp.x, max.x);
            max.y = std::max(tmp.y, max.y);
            max.z = std::max(tmp.z, max.z);

            const auto diff = max - min;
            const auto size2d = TVec2d(diff.x, diff.y);
//            const auto size_longer = std::max(size2d.x, size2d.y); // 縦と横のうち長い方の長さを採用します。非正方形の地形でテクスチャが引き伸ばされることを防ぎます。
            for(int i=0; i<vertices_count; i++) {
                const auto v_with_src_axis = vertices.at(i);
                const auto v = GeoReference::convertAxisToENU(geo_ref.getCoordinateSystem(), v_with_src_axis);
                const auto uv_x_norm = (float)((v.x - min.x) / size2d.x);
                const auto uv_y_norm = (float)((v.y - min.y) / size2d.y);
                const auto uv_norm = TVec2f(uv_x_norm, uv_y_norm);
                const auto uv_corrected = uv_min + uv_norm * (uv_max - uv_min);
                uv.push_back(uv_corrected);
            }
            mesh.setUV1(std::move(uv));
        }

        Extent getExtent(const TVec3d min, const TVec3d max, const GeoReference& geo_reference) { // NOLINT(performance-unnecessary-value-param)
            auto geo_min = geo_reference.unproject(min);
            auto geo_max = geo_reference.unproject(max);

            // 座標変換後に大小関係逆転することがあるため修正
            const auto tmp = geo_min;
            geo_min.latitude = std::min(tmp.latitude, geo_max.latitude);
            geo_min.longitude = std::min(tmp.longitude, geo_max.longitude);
            geo_min.height = std::min(tmp.height, geo_max.height);
            geo_max.latitude = std::max(tmp.latitude, geo_max.latitude);
            geo_max.longitude = std::max(tmp.longitude, geo_max.longitude);
            geo_max.height = std::max(tmp.height, geo_max.height);

            return {
                {geo_min.latitude, geo_min.longitude, -99999},
                { geo_max.latitude, geo_max.longitude, 99999}
            };
        }

    }

    int MapAttacher::attach(Model& model, const std::string& map_url_template, const std::filesystem::path& map_download_dest, const int zoom_level, const GeoReference& geo_reference) {
        int success_load_tile_count = 0;
        auto meshes = model.getAllMeshes();
        for(int i=0; i<meshes.size(); i++) {
            auto mesh = meshes.at(i);
            if(mesh->getVertices().empty()) continue;

            const auto [min, max] = mesh->calcBoundingBox();

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

            std::string combined_file_name;
            fs::path combined_image_path;

            // 同名のファイルがあったとき上書きしたくないので、同名でなくなるまでファイル名の番号を上げます
            for(int attempt=0; attempt<999; attempt++){
                // ファイル名は combined_map_mesh(mesh番号)_v(バージョン番号,基本は0だが同名のがすでにあれば増える)_p0 (pはpart、1枚にまとめるので基本0のはず)
                combined_file_name = "combined_map_mesh" + std::to_string(i) + "_v" + std::to_string(attempt) + "_p";
                // FIXME 下の ".png" の決め打ちはあまり良くない
                combined_image_path = fs::path(combined_tile_dir) / fs::u8path(combined_file_name + "0.png");
                if(!fs::exists(combined_image_path)){
                    break;
                }
            }
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
                    success_load_tile_count++;
                }
            }
            packer.flush();

            // 緯度経度を計算します。
            const auto image_extent = tiles.extent();
            const auto mesh_uv_min = image_extent.uvAt(extent.min);
            const auto mesh_uv_max = image_extent.uvAt(extent.max);

            // UVを貼ります。
            setUVForMap(*mesh, min, max, geo_reference, mesh_uv_min, mesh_uv_max);

            // テクスチャを結合後の地図タイルにします。
            for(auto& sub_mesh : mesh->getSubMeshes()) {
                if(!sub_mesh.getTexturePath().empty()) continue;
                sub_mesh.setTexturePath(combined_image_path.u8string());
            }
        }
        return success_load_tile_count;
    }
}
