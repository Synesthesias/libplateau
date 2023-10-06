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

    void setExpectNodeNameRange(std::string node_name, size_t begin_index, size_t last_index) {
        for(size_t i=begin_index; i<=last_index; i++) {
            expect_nodes_.at(i).expect_node_name_ = node_name;
        }
    }

    void setExpectGmlIdRange(std::string next_gml_id, size_t begin_index, size_t last_index) {
        for(size_t i=begin_index; i<=last_index; i++) {
            for(auto& [city_obj_index, gml_id] : expect_nodes_.at(i).expect_city_obj_list_ ) {
                gml_id = next_gml_id;
            }
        }
    }
    
private:
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
    /// テスト用モデル（地域単位）です。次のような構成です。
    /// gml_node <- lod_node <- mesh_node
    ModelConvertTestPatterns createTestModelOfArea();

    /// テスト用モデル（地域単位）のうち、gmlノードとlodノードをなくして、ルートノードに直接メッシュがあるパターンをテストします。
    ModelConvertTestPatterns createTestModelOfArea_OnlyRoot();
    /// テスト（主要地物）
    ModelConvertTestPatterns createTestModelOfPrimary();
    /// テスト（最小地物）
    ModelConvertTestPatterns createTestModelOfAtomic();

    /// テスト用モデル（地域単位）で、主要地物のメッシュがないバージョンです。
    ModelConvertTestPatterns createTestModelOfArea_OnlyAtomicMesh();
    /// テスト（主要地物）
    ModelConvertTestPatterns createTestModelOfAtomic_OnlyAtomicMesh();
    /// テスト（最小地物）
    ModelConvertTestPatterns createTestModelOfPrimary_OnlyAtomicMesh();

    /// テスト用モデル（地域単位）で、CityObjectIndexがないバージョンです。
    ModelConvertTestPatterns createTestModelOfArea_WithoutCityObjList();
    ModelConvertTestPatterns createTestModelOfPrimary_WithoutCityObjList();
    ModelConvertTestPatterns createTestModelOfAtomic_WithoutCityObjList();

private:
    std::unique_ptr<plateau::polygonMesh::Mesh> createTestMeshOfArea(std::vector<plateau::polygonMesh::CityObjectIndex> city_object_indices, plateau::polygonMesh::CityObjectList city_obj_list);
    ModelConvertTestPatterns::TGranularityToExpect createExpectsForTestMeshArea();

    const std::vector<plateau::polygonMesh::CityObjectIndex> test_indices_primary_and_atomic = {
            {0, -1}, {0,0}, {0,1},
            {1, 0}, {1, 1}, {1, -1}
    };

    const plateau::polygonMesh::CityObjectList test_city_obj_list_primary_and_atomic = {
            {
                {{0, -1}, "primary-0"},
                {{0, 0}, "atomic-0-0"},
                {{0,1}, "atomic-0-1"},
                {{1,-1}, "primary-1"},
                {{1,0}, "atomic-1-0"},
                {{1,1}, "atomic-1-1"}
    }};
    const std::vector<plateau::polygonMesh::CityObjectIndex> test_indices_only_atomic = {
            {0, 0}, {0, 1},
            {1, 0}, {1, 1}
    };

    const plateau::polygonMesh::CityObjectList test_city_obj_list_indices_only_atomic = {
            {
                    {{0,0}, "atomic-0-0"},
                    {{0,1}, "atomic-0-1"},
                    {{1,0}, "atomic-1-0"},
                    {{1,1}, "atomic-1-1"}
            }
    };
};
