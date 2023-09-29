#pragma once
#include "plateau/polygon_mesh/model.h"

/// ModelのNode構成がどのようなものになっていると期待するかです。
/// 各Nodeに期待する状態が、幅優先探索の順番でvectorで表現されます。
class ModelExpect {
    using TExpectNodesName = std::vector<std::string>;
    using TExpectHaveMesh = std::vector<bool>;
    using TExpectVerticesCount = std::vector<int>;
    using TExpectCityObjsIdSet = std::vector<std::set<plateau::polygonMesh::CityObjectIndex>>;
    using TExpectCityObjsList = std::vector<plateau::polygonMesh::CityObjectList>;
    using TExpectSubMeshesList = std::vector<std::vector<plateau::polygonMesh::SubMesh>>;
public:
    ModelExpect(
            TExpectNodesName  expect_nodes_name,
            TExpectHaveMesh  expect_have_mesh,
            TExpectVerticesCount  expect_vertices_count,
            TExpectCityObjsIdSet  expect_city_objs_id_set,
            TExpectCityObjsList  expect_city_objs_lists,
            TExpectSubMeshesList  expect_sub_meshes_list
    ) :
            expect_nodes_name_(std::move(expect_nodes_name)),
            expect_have_mesh_(std::move(expect_have_mesh)),
            expect_vertices_count_(std::move(expect_vertices_count)),
            expect_city_objs_id_set_(std::move(expect_city_objs_id_set)),
            expect_city_objs_list_(std::move(expect_city_objs_lists)),
            expect_sub_meshes_list_(std::move(expect_sub_meshes_list))
    {

    };

    size_t nodeSize() const;

    const TExpectNodesName expect_nodes_name_;
    const TExpectHaveMesh expect_have_mesh_;
    const TExpectVerticesCount expect_vertices_count_; // expect_vertex_countとexpect_city_obj_idはMeshを持たないノードに関しては無視されます。
    const TExpectCityObjsIdSet expect_city_objs_id_set_;
    const TExpectCityObjsList expect_city_objs_list_;
    const TExpectSubMeshesList expect_sub_meshes_list_;
};


/**
 * 変換前のテストモデルと、それが変換後どうあるべきか（粒度別）を保持します。
 * また実際に変換し、両者を比較します。
 */
class ModelForTest {
public:
    using TGranularityToExpect = std::map<plateau::polygonMesh::MeshGranularity, ModelExpect>;
    ModelForTest(plateau::polygonMesh::Model&& src_model, TGranularityToExpect& convert_expects) :
            src_model_(std::move(src_model)),
            convert_expects_(convert_expects) {};
    void test();
private:
    plateau::polygonMesh::Model src_model_;
    TGranularityToExpect convert_expects_;
};
