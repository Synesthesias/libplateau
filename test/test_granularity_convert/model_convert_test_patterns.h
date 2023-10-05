#pragma once
#include <utility>

#include "plateau/polygon_mesh/model.h"

/// Nodeの状態がどのようなものと期待するかです。
class NodeExpect {
public:
    NodeExpect(
            const std::string& node_name,
            bool have_mesh,
            int vertices_count,
            const std::set<plateau::polygonMesh::CityObjectIndex>& city_obj_id_set,
            const plateau::polygonMesh::CityObjectList& city_obj_list,
            const std::vector<plateau::polygonMesh::SubMesh>& sub_meshes) :
            expect_node_name_(node_name), expect_have_mesh_(have_mesh),
            expect_vertex_count_(vertices_count), expect_city_obj_id_set_(city_obj_id_set),
            expect_city_obj_list_(city_obj_list), expect_sub_meshes_(sub_meshes)
            {};
    void checkNode(const plateau::polygonMesh::Node* node) const;

    std::string expect_node_name_;
    bool expect_have_mesh_;
    int expect_vertex_count_;
    std::set<plateau::polygonMesh::CityObjectIndex> expect_city_obj_id_set_;
    plateau::polygonMesh::CityObjectList expect_city_obj_list_;
    std::vector<plateau::polygonMesh::SubMesh> expect_sub_meshes_;
};

/// ModelのNode構成がどのようなものになっていると期待するかです。
/// 各Nodeに期待する状態が、幅優先探索の順番でvectorで表現されます。
class ModelExpect {
public:
    ModelExpect(std::vector<NodeExpect>  expect_nodes) :
        expect_nodes_(std::move(expect_nodes))
    {

    };

    NodeExpect& at(size_t i) {return expect_nodes_.at(i);};
    const NodeExpect& at(size_t i) const {return expect_nodes_.at(i);};

    size_t nodeCount() const {return expect_nodes_.size();};

    void eraseRange(int start, int last) {
        auto begin = expect_nodes_.begin();
        expect_nodes_.erase(begin+start, begin+last);
    }

    std::vector<NodeExpect> expect_nodes_;
};


/**
 * 変換前のテストモデルと、それが変換後どうあるべきか（粒度別）を保持します。
 * また実際に変換し、両者を比較します。
 */
class ModelConvertTestPatterns {
public:

    /// 変換先の粒度と、変換後に期待する値のmapです。
    using TGranularityToExpect = std::map<plateau::polygonMesh::MeshGranularity, ModelExpect>;

    ModelConvertTestPatterns(plateau::polygonMesh::Model&& src_model, TGranularityToExpect& convert_expects) :
            src_model_(std::move(src_model)),
            convert_expects_(convert_expects) {};
    void test(plateau::polygonMesh::MeshGranularity granularity);
    plateau::polygonMesh::Model& getModel() {return src_model_;};
    TGranularityToExpect& getExpects(){return convert_expects_;};
private:
    plateau::polygonMesh::Model src_model_;
    TGranularityToExpect convert_expects_;
};

class ModelConvertTestPatternsFactory {
public:
    /// gml_node <- lod_node <- mesh_node
    ModelConvertTestPatterns createTestModelOfArea();

    /// 地域単位のテスト用モデルのうち、gmlノードとlodノードをなくして、ルートノードに直接メッシュがあるパターンをテストします。
    ModelConvertTestPatterns createTestModelOfArea_OnlyRoot();
    ModelConvertTestPatterns createTestModelOfPrimary();
    ModelConvertTestPatterns createTestModelOfAtomic();

    /// テスト用モデルで、主要地物のメッシュがないバージョンです。
    ModelConvertTestPatterns createTestModelOfArea_OnlyAtomicMesh();
    ModelConvertTestPatterns createTestModelOfAtomic_OnlyAtomicMesh();
    ModelConvertTestPatterns createTestModelOfPrimary_OnlyAtomicMesh();
private:
    std::unique_ptr<plateau::polygonMesh::Mesh> createTestMeshOfArea(std::vector<plateau::polygonMesh::CityObjectIndex> city_object_indices);
    ModelConvertTestPatterns::TGranularityToExpect createExpectsForTestMeshArea();
    const std::vector<plateau::polygonMesh::CityObjectIndex> test_indices_primary_and_atomic = {
            {0, -1}, {0,0}, {0,1},
            {1, 0}, {1, 1}, {1, -1}
    };
    const std::vector<plateau::polygonMesh::CityObjectIndex> test_indices_only_atomic = {
            {0, 0}, {0, 1},
            {1, 0}, {1, 1}
    };
};
