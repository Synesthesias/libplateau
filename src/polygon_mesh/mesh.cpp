#include <plateau/polygon_mesh/mesh.h>
#include <memory>
#include "citygml/texture.h"
#include "citygml/cityobject.h"
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include "../io/polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"
#include <stdexcept>

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

    const std::vector<int>& Mesh::getIndices() const {
        return indices_;
    }

    void Mesh::setUV2(const UV& uv2) {
        if (uv2.size() != vertices_.size()) {
            throw std::exception("Size of uv2 does not match num of vertices.");
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

    void Mesh::addVerticesList(const std::vector<TVec3d>& other_vertices, const MeshExtractOptions& options) {
        // 各頂点を座標変換しながら追加します。
        for (const auto& other_pos: other_vertices) {
            // other_pos は極座標系です。
            auto pos = other_pos;
            // デカルト座標系に直します。
            // FIXME 変換部分だけ ObjWriterの機能を拝借していますが、本質的には ObjWriter である必要はないです。変換を別クラスに書き出した方が良いです。
            // TODO 座標変換については、今後は自作せずライブラリを利用する方針になったのでライブラリ導入後にここを直します。
            polar_to_plane_cartesian().convert(pos);
            // オプションに応じて座標系を変更します。
            pos = ObjWriter::convertPosition(pos, options.reference_point, options.mesh_axes, options.unit_scale);
            vertices_.push_back(pos);
        }
    }

    void Mesh::addIndicesList(const std::vector<unsigned>& other_indices, unsigned prev_num_vertices) {
        auto prev_num_indices = indices_.size();

        // インデックスリストの末尾に追加します。
        for (unsigned int other_index: other_indices) {
            indices_.push_back((int) other_index);
        }
        // 追加分のインデックスを新しい値にします。以前の頂点の数だけインデックスの数値を大きくすれば良いです。
        for (unsigned i = prev_num_indices; i < indices_.size(); i++) {
            indices_.at(i) += (int) prev_num_vertices;
        }
    }

    void Mesh::addUV1(const Polygon& other_poly) {
        // UV1を追加します。
        auto& other_uv_1 = other_poly.getTexCoordsForTheme("rgbTexture", true);
        for (const auto& vec: other_uv_1) {
            uv1_.push_back(vec);
        }
        // other_uv_1 の数が頂点数に足りなければ 0 で埋めます。
        auto other_vertices_size = other_poly.getVertices().size();
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

    void Mesh::addSubMesh(const std::string& texture_path, size_t sub_mesh_indices_size) {
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
            unsigned prev_num_indices = indices_.size() - sub_mesh_indices_size;
            SubMesh::addSubMesh(prev_num_indices, indices_.size() - 1, texture_path, sub_meshes_);
        } else {
            // テクスチャが同じなら、最後のサブメッシュの範囲を延長して新しい部分の終わりに合わせます。
            extendLastSubMesh();
        }
    }

    void Mesh::extendLastSubMesh() {
        if (sub_meshes_.empty()) {
            sub_meshes_.emplace_back(0, indices_.size() - 1, "");
        } else {
            sub_meshes_.at(sub_meshes_.size() - 1).setEndIndex((int) indices_.size() - 1);
        }
    }
}
