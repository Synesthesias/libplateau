#include <plateau/polygon_mesh/mesh.h>
#include <plateau/polygon_mesh/mesh_merger.h>

#include "citygml/texture.h"
#include "citygml/cityobject.h"
#include "plateau/geometry/geo_reference.h"

namespace plateau::polygonMesh {
    using namespace citygml;

    Mesh::Mesh()
        : uv1_(UV())
        , uv4_(UV()) {
    }

    Mesh::Mesh(std::vector<TVec3d>&& vertices, std::vector<unsigned>&& indices, UV&& uv_1, UV&& uv_4,
               std::vector<SubMesh>&& sub_meshes, CityObjectList&& city_object_list)
        : vertices_(std::move(vertices))
        , indices_(std::move(indices))
        , uv1_(std::move(uv_1))
        , uv4_(std::move(uv_4))
        , sub_meshes_(std::move(sub_meshes))
        , city_object_list_(std::move(city_object_list))
        , vertex_colors_(){
    }

    std::vector<TVec3d>& Mesh::getVertices() {
        return vertices_;
    }

    const std::vector<TVec3d>& Mesh::getVertices() const {
        return vertices_;
    }

    const std::vector<unsigned>& Mesh::getIndices() const {
        return indices_;
    }

    std::vector<unsigned>& Mesh::getIndices() {
        return indices_;
    }

    const UV& Mesh::getUV1() const {
        return uv1_;
    }

    UV& Mesh::getUV1() {
        return uv1_;
    }

    const UV& Mesh::getUV4() const {
        return uv4_;
    }

    UV& Mesh::getUV4() {
        return uv4_;
    }

    const std::vector<SubMesh>& Mesh::getSubMeshes() const {
        return sub_meshes_;
    }

    std::vector<SubMesh>& Mesh::getSubMeshes() {
        return sub_meshes_;
    }

    const std::vector<TVec3d>& Mesh::getVertexColors() const {
        return vertex_colors_;
    }

    void Mesh::setVertexColors(std::vector<TVec3d>& vertex_colors) {
        vertex_colors_ = vertex_colors;
    }

    void Mesh::setSubMeshes(std::vector<SubMesh>& sub_mesh_list) {
        sub_meshes_ = sub_mesh_list;
    }

    void Mesh::setSubMeshes(std::vector<SubMesh>&& sub_mesh_list) {
        sub_meshes_ = std::move(sub_mesh_list);
    }

    void Mesh::reserve(long long vertex_count) {
        vertices_.reserve(vertex_count);
        indices_.reserve(vertex_count);
        uv1_.reserve(vertex_count);
        uv4_.reserve(vertex_count);
    }

    void Mesh::addVerticesList(const std::vector<TVec3d>& other_vertices) {
        // 各頂点を追加します。
        for (const auto& other_pos : other_vertices) {
            vertices_.push_back(other_pos);
        }
    }

    /**
     * Indices を追加します。
     * メッシュのマージ処理の流れについて
     * [1]元のメッシュ　→　[2]頂点追加　→　[3]Indices追加 → [4]UV等追加
     * のうち [3] をこの関数が担当します。
     * 引数の prev_num_vertices には、[1]の段階の頂点数（頂点追加する前の頂点数がいくつであったか）を渡します。
     * この値は、追加する indices の値のオフセットとなります。
     */
    void Mesh::addIndicesList(const std::vector<unsigned>& other_indices, unsigned prev_num_vertices,
                              bool invert_mesh_front_back) {
        auto prev_num_indices = indices_.size();

        if (other_indices.size() % 3 != 0) {
            throw std::runtime_error("size of other_indices must be multiple of 3.");
        }

        // インデックスリストの末尾に追加します。
        // 以前の頂点の数だけインデックスの数値を大きくします。
        for (auto other_index : other_indices) {
            indices_.push_back(other_index + (int)prev_num_vertices);
        }

        // メッシュを裏返すべきとき、次の方法で裏返します:
        // indices を 3つごとに分け、三角形グループとします。
        // 各三角形グループ内の順番を反転させます。
        if (invert_mesh_front_back) {
            auto triangle_count = other_indices.size() / 3;
            for (int tri = 0; tri < triangle_count; tri++) {
                auto vert1ID = prev_num_indices + 3 * tri;
                auto vert3ID = vert1ID + 2;
                auto vert1 = indices_.at(vert1ID);
                auto vert3 = indices_.at(vert3ID);
                indices_.at(vert1ID) = vert3;
                indices_.at(vert3ID) = vert1;
            }
        }
    }


    void Mesh::setUV1(UV&& uv) {
        uv1_ = std::move(uv);
    }

    void Mesh::setUV4(UV&& uv4) {
        uv4_ = std::move(uv4);
    }

    void Mesh::addUV1(const std::vector<TVec2f>& other_uv_1, unsigned long long other_vertices_size) {
        // UV1を追加します。
        for (const auto& vec : other_uv_1) {
            uv1_.push_back(vec);
        }
        // other_uv_1 の数が頂点数に足りなければ 0 で埋めます。
        for (size_t i = other_uv_1.size(); i < other_vertices_size; i++) {
            uv1_.emplace_back(0, 0);
        }
    }

    void Mesh::addUV4(const std::vector<TVec2f>& other_uv_4, unsigned long long other_vertices_size) {
        // UV4を追加します。
        for (const auto& vec : other_uv_4) {
            uv4_.push_back(vec);
        }
    }

    void Mesh::addUV4WithSameVal(const TVec2f& uv_4_val, const long long size) {
        for (int i = 0; i < size; i++) {
            uv4_.push_back(uv_4_val);
        }
    }

    void Mesh::addSubMesh(const std::string& texture_path, std::shared_ptr<const citygml::Material> material, size_t sub_mesh_start_index, size_t sub_mesh_end_index, int game_material_id) {
        // テクスチャが異なる場合は追加します。

        // 前と同じマテリアルかどうか判定します。
        bool are_materials_same;
        if (sub_meshes_.empty()) {
            are_materials_same = false;
        } else {
            const auto& last_sub_mesh = sub_meshes_.rbegin();
            are_materials_same =
                    SubMesh(sub_mesh_start_index, sub_mesh_end_index, texture_path, material, game_material_id)
                    .isSameAs(*last_sub_mesh);
        }

        if (!are_materials_same) {
            // テクスチャが違うなら、サブメッシュを追加します。
            SubMesh::addSubMesh(sub_mesh_start_index, sub_mesh_end_index, texture_path, material, game_material_id, sub_meshes_);
        } else {
            // テクスチャが同じなら、最後のサブメッシュの範囲を延長して新しい部分の終わりに合わせます。
            extendLastSubMesh(sub_mesh_end_index);
        }
    }

    void Mesh::extendLastSubMesh(size_t sub_mesh_end_index) {
        if (sub_meshes_.empty()) {
            sub_meshes_.emplace_back(0, sub_mesh_end_index, "", nullptr);
        } else {
            sub_meshes_.at(sub_meshes_.size() - 1).setEndIndex(sub_mesh_end_index);
        }
    }

    void Mesh::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "Mesh: ( " << vertices_.size() << " vertices, " << indices_.size() << " indices )" << std::endl;
        for (const auto& sub_mesh : sub_meshes_) {
            sub_mesh.debugString(ss, indent + 1);
        }
    }

    std::tuple<TVec3d, TVec3d> Mesh::calcBoundingBox() const {
        constexpr double double_min = std::numeric_limits<double>::lowest();
        constexpr double double_max = std::numeric_limits<double>::infinity();
        auto min = TVec3d(double_max, double_max, double_max);
        auto max = TVec3d(double_min, double_min, double_min);
        auto& vertices = getVertices();
        auto vertices_count = vertices.size();
        for(int i=0; i<vertices_count; i++) {
            const auto pos3d = vertices.at(i);
            min.x = std::min(min.x, pos3d.x);
            min.y = std::min(min.y, pos3d.y);
            min.z = std::min(min.z, pos3d.z);
            max.x = std::max(max.x, pos3d.x);
            max.y = std::max(max.y, pos3d.y);
            max.z = std::max(max.z, pos3d.z);
        }
        return {min, max};
    }

    bool Mesh::hasVertices() const {
        return !vertices_.empty();
    }

    void Mesh::merge(const Mesh& other_mesh, const bool invert_mesh_front_back, const bool include_textures) {
        MeshMerger::mergeMesh(*this, other_mesh, invert_mesh_front_back, include_textures);
    }

    void Mesh::combineSameSubMeshes() {
        // SubMeshの集合を構築します。indexは無視して見た目のみチェックします。
        std::set<SubMesh, SubMeshCompareByAppearance> sm_set;
        bool contains_duplicate = false;
        for(auto& sm : sub_meshes_) {
            if(sm_set.find(sm) == sm_set.end()) {
                sm_set.insert(sm);
            }else{
                contains_duplicate = true;
            }
        }
        // 重複SubMeshがなければ処理不要です。
        if(!contains_duplicate) return;

        std::vector<unsigned int> next_indices;
        std::vector<SubMesh> next_sub_meshes;
        next_indices.reserve(indices_.size());
        next_sub_meshes.reserve(sm_set.size());
        size_t sm_start_index = 0;
        for(auto& unique_sm : sm_set) {
            for(auto& sm : sub_meshes_) {
                if(sm.isAppearanceEqual(unique_sm)) {
                    for(auto i = sm.getStartIndex(); i <= sm.getEndIndex(); i++) {
                        next_indices.push_back(indices_.at(i));
                    }
                }
            }
            auto next_sm = unique_sm;
            next_sm.setStartIndex(sm_start_index);
            auto last_index = next_indices.size() - 1;
            next_sm.setEndIndex(last_index);
            next_sub_meshes.push_back(next_sm);
            sm_start_index = last_index + 1;
        }
        indices_ = std::move(next_indices);
        sub_meshes_ = std::move(next_sub_meshes);
    }

    const CityObjectList& Mesh::getCityObjectList() const {
        return city_object_list_;
    }

    CityObjectList& Mesh::getCityObjectList() {
        return city_object_list_;
    }

    void Mesh::setCityObjectList(const CityObjectList& city_obj_list) {
        city_object_list_ = city_obj_list;
    }

    void Mesh::convertAxisToENUFrom(geometry::CoordinateSystem from_axis) {
        for(auto& vertex : vertices_) {
            vertex = geometry::GeoReference::convertAxisToENU(from_axis, vertex);
        }
    }

    void Mesh::convertAxisFromENUTo(geometry::CoordinateSystem to_axis) {
        for(auto& vertex : vertices_) {
            vertex = geometry::GeoReference::convertAxisFromENUTo(to_axis, vertex);
        }
    }
}
