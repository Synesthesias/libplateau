#include <plateau/polygon_mesh/map_attacher.h>

namespace {

}

namespace plateau::polygonMesh {

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
        void setUVForMap(Mesh& mesh) {
            auto [min, max] = calcBoundingBox2D(mesh);
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
    }

    void MapAttacher::attach(Model& model, const GeoReference& geo_reference) {
        auto meshes = model.getAllMeshes();
        for(auto mesh : meshes) {
            setUVForMap(*mesh);
        }
    }
}
