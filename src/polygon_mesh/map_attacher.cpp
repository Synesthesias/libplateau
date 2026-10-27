#include <plateau/polygon_mesh/map_attacher.h>
#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/texture/texture_image_base.h>
#include <plateau/texture/texture_packer.h>
#include <cassert>
#include <cmath>

namespace plateau::polygonMesh {
    namespace fs = std::filesystem;
    using namespace plateau::texture;

    namespace {


        /// ウェブメルカトルの南北成分です。緯度が高いほど大きくなります。
        /// 地理院タイルの Y 計算（TileProjection）と同じ投影です。
        double webMercatorY(const double latitude_deg) {
            constexpr double deg_to_rad = 0.017453292519943295;
            const double lat_rad = latitude_deg * deg_to_rad;
            return std::asinh(std::tan(lat_rad));
        }

        /// 結合した地図タイル画像へ、頂点ごとのUVを設定します。
        /// 画像の西端が U=0、東端が U=1、南端が V=0、北端が V=1 です。
        /// 経度はタイル上で線形、緯度はウェブメルカトルです。
        /// メッシュの外接矩形で線形補間すると、3次メッシュ境界の同じ頂点が隣のメッシュと別の画素を指します。
        void setUVForMap(Mesh& mesh, const GeoReference& geo_ref, const Extent& image_extent) {
            const auto& vertices = mesh.getVertices();
            const auto vertices_count = vertices.size();
            auto uv = std::vector<TVec2f>();
            uv.reserve(vertices_count);

            const auto longitude_min = image_extent.min.longitude;
            const auto longitude_span = image_extent.max.longitude - longitude_min;
            const auto mercator_south = webMercatorY(image_extent.min.latitude);
            const auto mercator_span = webMercatorY(image_extent.max.latitude) - mercator_south;

            for(int i=0; i<vertices_count; i++) {
                const auto geo = geo_ref.unproject(vertices.at(i));
                const auto uv_x = static_cast<float>((geo.longitude - longitude_min) / longitude_span);
                const auto uv_y = static_cast<float>((webMercatorY(geo.latitude) - mercator_south) / mercator_span);
                uv.push_back(TVec2f(uv_x, uv_y));
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

            // 結合画像の範囲へ、頂点ごとにUVを貼ります。
            setUVForMap(*mesh, geo_reference, tiles.extent());

            // テクスチャを結合後の地図タイルにします。
            for(auto& sub_mesh : mesh->getSubMeshes()) {
                if(!sub_mesh.getTexturePath().empty()) continue;
                sub_mesh.setTexturePath(combined_image_path.u8string());
            }
        }
        return success_load_tile_count;
    }
}
