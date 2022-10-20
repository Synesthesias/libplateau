#include <plateau/polygon_mesh/mesh.h>
#include <memory>
#include "citygml/texture.h"
#include "citygml/cityobject.h"
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include "plateau/mesh_writer/obj_writer.h"

namespace plateau::polygonMesh {
    using namespace citygml;

    Mesh::Mesh(const std::string& id) :
            vertices_(),
            uv1_(UV()),
            uv2_(UV()),
            uv3_(UV()),
            sub_meshes_() {
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

    void Mesh::setUV2(const UV& uv2) {
        if (uv2.size() != vertices_.size()) {
            throw std::logic_error("Size of uv2 does not match num of vertices.");
        }
        uv2_ = uv2;
    }

    const UV& Mesh::getUV1() const {
        return uv1_;
    }

    const UV& Mesh::getUV2() const {
        return uv2_;
    }

    const UV& Mesh::getUV3() const {
        return uv3_;
    }

    const std::vector<SubMesh>& Mesh::getSubMeshes() const {
        return sub_meshes_;
    }

    void Mesh::addVerticesList(const std::vector<TVec3d>& other_vertices) {
        // 各頂点を追加します。
        for (const auto& other_pos: other_vertices) {
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

        if(other_indices.size() % 3 != 0){
            throw std::runtime_error("size of other_indices must be multiple of 3.");
        }

        // TODO この2つの for は1つの forにまとめられるはず
        // インデックスリストの末尾に追加します。
        for (auto other_index: other_indices) {
            indices_.push_back(other_index);
        }
        // 追加分のインデックスを新しい値にします。以前の頂点の数だけインデックスの数値を大きくすれば良いです。
        for (unsigned i = prev_num_indices; i < indices_.size(); i++) {
            indices_.at(i) += (int)prev_num_vertices;
        }

        // メッシュを裏返すべきとき、次の方法で裏返します:
        // indices を 3つごとに分け、三角形グループとします。
        // 各三角形グループ内の順番を反転させます。
        if(invert_mesh_front_back){
            auto triangle_count = other_indices.size() / 3;
            for(int tri = 0; tri < triangle_count; tri++){
                auto vert1ID = prev_num_indices + 3 * tri;
                auto vert3ID = vert1ID + 2;
                auto vert1 = indices_.at(vert1ID);
                auto vert3 = indices_.at(vert3ID);
                indices_.at(vert1ID) = vert3;
                indices_.at(vert3ID) = vert1;
            }
        }
    }

    void Mesh::addUV1(const std::vector<TVec2f>& other_uv_1, unsigned long long other_vertices_size) {
        // UV1を追加します。
        for (const auto& vec: other_uv_1) {
            uv1_.push_back(vec);
        }
        // other_uv_1 の数が頂点数に足りなければ 0 で埋めます。
        for (size_t i = other_uv_1.size(); i < other_vertices_size; i++) {
            uv1_.emplace_back(0, 0);
        }
    }

    void Mesh::addUV2WithSameVal(const TVec2f& uv_2_val, unsigned num_adding_vertices) {
        for (int i = 0; i < num_adding_vertices; i++) {
            uv2_.push_back(uv_2_val);
        }
    }

    void Mesh::addUV3WithSameVal(const TVec2f& uv_3_val, unsigned num_adding_vertices) {
        for (int i = 0; i < num_adding_vertices; i++) {
            uv3_.push_back(uv_3_val);
        }
    }

    void Mesh::addSubMesh(const std::string& texture_path, size_t sub_mesh_start_index, size_t sub_mesh_end_index) {
        // テクスチャが異なる場合は追加します。
        // TODO テクスチャありのポリゴン と なしのポリゴン が交互にマージされることで、テクスチャなしのサブメッシュが大量に生成されるので描画負荷に改善の余地ありです。
        //      テクスチャなしのサブメッシュは1つにまとめたいところです。テクスチャなしのポリゴンを連続してマージすることで1つにまとまるはずです。

        // 前と同じテクスチャかどうか判定します。
        bool is_different_tex;
        if (sub_meshes_.empty()) {
            is_different_tex = true;
        } else {
            auto& last_texture_path = sub_meshes_.rbegin()->getTexturePath();
            is_different_tex = texture_path != last_texture_path;
        }

        if (is_different_tex) {
            // テクスチャが違うなら、サブメッシュを追加します。
//            unsigned prev_num_indices = indices_.size() - sub_mesh_indices_size;
            SubMesh::addSubMesh(sub_mesh_start_index, sub_mesh_end_index/*indices_.size() - 1*/, texture_path, sub_meshes_);
        } else {
            // テクスチャが同じなら、最後のサブメッシュの範囲を延長して新しい部分の終わりに合わせます。
            extendLastSubMesh(sub_mesh_end_index);
        }
    }

    void Mesh::extendLastSubMesh(size_t sub_mesh_end_index) {
        if (sub_meshes_.empty()) {
            sub_meshes_.emplace_back(0, sub_mesh_end_index/*indices_.size() - 1*/, "");
        } else {
            sub_meshes_.at(sub_meshes_.size() - 1).setEndIndex(sub_mesh_end_index/*(int) indices_.size() - 1*/);
        }
    }
}
