#include <plateau/polygon_mesh/map_attacher.h>
#include <plateau/basemap/tile_projection.h>
#include <plateau/basemap/vector_tile_downloader.h>

namespace plateau::polygonMesh {
    namespace fs = std::filesystem;

    namespace {
        /// メッシュのBoundingBoxを計算します。ただし2D(東西南北方向)のみで、高さは無視します。
        std::tuple<TVec2d, TVec2d> calcBoundingBox2D(Mesh& mesh) {
            auto min = TVec2(DBL_MAX, DBL_MAX);
            auto max = TVec2(DBL_MIN, DBL_MIN);
            auto& vertices = mesh.getVertices();
            auto vertices_count = vertices.size();
            for(int i=0; i<vertices_count; i++) {
                auto pos3d = vertices.at(i);
                min.x = std::min(min.x, pos3d.x);
                min.y = std::min(min.y, pos3d.z);
                max.x = std::max(max.x, pos3d.x);
                max.y = std::max(max.y, pos3d.z);
            }
            return {min, max};
        }

        /// 地図タイルを貼り付ける準備として、メッシュにUVを設定します。
        /// メッシュの頂点のうち、もっとも南西にある箇所をUV(0,0)とします。
        /// メッシュを包む正方形の中でもっとも北東の箇所をUV(1,1)とします。
        /// メッシュが正方形の場合、メッシュの北東端は UV(1,1)です。
        /// メッシュが東西方向に長い場合、メッシュの北東端は正方形の右の辺上にあるので UV(1,y) (0<y<1) です。
        /// メッシュが南北方向に長い場合、メッシュの北東端は正方形の上の辺上にあるので UV(x,1) (0<x<1) です。
        /// 頂点の高さは考慮しません。
        void setUVForMap(Mesh& mesh, const TVec2d min, const TVec2d max) { // NOLINT(performance-unnecessary-value-param)
            auto& vertices = mesh.getVertices();
            auto vertices_count = vertices.size();
            auto uv = std::vector<TVec2f>();
            uv.reserve(vertices_count);
            auto size2d = max - min;
            auto size_longer = std::max(size2d.x, size2d.y); // 縦と横のうち長い方の長さを採用します。非正方形の地形でテクスチャが引き伸ばされることを防ぎます。
            for(int i=0; i<vertices_count; i++) {
                auto v = vertices.at(i);
                auto uv_x = (float)((v.x - min.x) / size_longer);
                auto uv_y = (float)((v.z - min.y) / size_longer);
                uv.emplace_back(uv_x, uv_y);
            }
            mesh.setUV1(std::move(uv));
        }

        Extent getExtent(const TVec2d min, const TVec2d max, const GeoReference& geo_reference) { // NOLINT(performance-unnecessary-value-param)
            const auto geo_min = geo_reference.unproject(TVec3d(min.x, -99999, min.y));
            const auto geo_max = geo_reference.unproject(TVec3d(max.x, 99999, max.y));
            return {geo_min, geo_max};
        }

        /**
         * VectorTileDownloaderを利用して地図をダウンロードします。
         * 戻り値はダウンロードした画像パスの配列です。ただし、ダウンロードに失敗した分は空文字列が入ります。
         */
        std::vector<std::string> download(const VectorTileDownloader& downloader) {
            auto downloaded_path = std::vector<std::string>();
            auto count = downloader.getTileCount();
            for(int i=0; i<count; i++) {
                auto tile = downloader.download(i);
                auto path = std::string("");
                if(tile->result == HttpResult::Success) {
                    path = tile->image_path;
                }
                downloaded_path.push_back(path);
            }
            return downloaded_path;
        }

    }

    void MapAttacher::attach(Model& model, const std::string& map_url_template, const std::filesystem::path& map_download_dest, const int zoom_level, const GeoReference& geo_reference) {
        auto meshes = model.getAllMeshes();
        for(auto mesh : meshes) {
            const auto [min, max] = calcBoundingBox2D(*mesh);
            setUVForMap(*mesh, min, max);
            auto extent = getExtent(min, max, geo_reference);
            auto downloader = VectorTileDownloader(map_download_dest.u8string(), extent, zoom_level);
            downloader.setUrl(map_url_template);
            auto image_paths = download(downloader);
        }
    }
}
