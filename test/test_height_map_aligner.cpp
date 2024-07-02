#include "gtest/gtest.h"
#include "citygml/citygml.h"
#include "plateau/height_map_alighner/height_map_aligner.h"
#include <plateau/height_map_generator/heightmap_types.h>
#include <memory>
#include <cmath>

using namespace plateau::heightMapAligner;
using namespace plateau::heightMapGenerator;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

class HeightMapAlignerTest : public ::testing::Test {

};

TEST_F(HeightMapAlignerTest, AlignOnZeroHeightMap) { // NOLINT
    auto aligner = HeightMapAligner(0, plateau::geometry::CoordinateSystem::ENU);
    auto model = plateau::polygonMesh::Model();
    auto frame = HeightMapFrame(std::vector<uint16_t>(), 0, 0, 0, 0, 0, 0, 0, 0, plateau::geometry::CoordinateSystem::ENU);
    aligner.addHeightmapFrame(frame);
    aligner.align(model, 4);
}

TEST_F(HeightMapAlignerTest, Align) { // NOLINT
    // ポリゴンが1つだけのモデルを作成します
    auto model = Model();
    auto node = Node("test_node");
    auto src_vertices = std::vector<TVec3d>{{0,0,-1}, {0,1, -1}, {1,1,-1}};
    auto src_indices = std::vector<unsigned>{0, 1, 2};
    auto src_uv1 = UV{{0, 0}, {0,0}, {0,0}};
    auto src_uv4 = UV{{0,0}, {0,0}, {0,0}};
    auto src_sub_meshes = std::vector<SubMesh>{SubMesh(0, 2, "", nullptr, 1)};
    auto src_mesh = std::make_unique<Mesh>(
            std::vector<TVec3d>(src_vertices),
            std::vector<unsigned>(src_indices),
            std::vector<TVec2f>(src_uv1),
            std::vector<TVec2f>(src_uv4),
            std::vector<SubMesh>(src_sub_meshes),
            CityObjectList());
    node.setMesh(std::move(src_mesh));
    model.addNode(std::move(node));

    // alignを実行します
    auto height_map = HeightMapT{0};
    auto height_map_frame = HeightMapFrame(height_map, 1, 1, 0, 1, 0, 1, 1, 2, CoordinateSystem::ENU);
    auto aligner = HeightMapAligner(0.5, CoordinateSystem::ENU);
    aligner.addHeightmapFrame(height_map_frame);
    aligner.align(model, 999);

    // 実行結果をチェックします
    auto dst_mesh = model.getRootNodeAt(0).getMesh();

    // 頂点チェック
    auto dst_vertices = dst_mesh->getVertices();
    ASSERT_EQ(src_vertices.size(), dst_vertices.size()) << "alignしても頂点数は同じ";
    auto expect_vertices = std::vector<TVec3d>{{0,0,1.5}, {0,1,1.5}, {1,1,1.5}};
    for(int i=0; i<src_vertices.size(); i++) {
        const auto dist = (dst_vertices.at(i) - expect_vertices.at(i)).length();
        ASSERT_TRUE(dist < 0.0001) << "alignによる頂点の高さの変化";
    }

    // indicesチェック
    auto dst_indices = dst_mesh->getIndices();
    ASSERT_EQ(src_indices.size(), dst_indices.size()) << "alignしても三角形数は同じ";
    for(int i=0; i<src_indices.size(); i++) {
        ASSERT_EQ(dst_indices.at(i), src_indices.at(i)) << "alignしても三角形の番号は変わらない";
    }

    // UV1チェック
    auto dst_uv1 = dst_mesh->getUV1();
    ASSERT_EQ(src_uv1.size(), dst_uv1.size()) << "alignしてもUV1の数は同じ";
    for(int i=0; i<src_uv1.size(); i++) {
        ASSERT_EQ(dst_uv1.at(i), src_uv1.at(i)) << "alignしてもUV1の値は変わらない";
    }

    // UV4チェック
    auto dst_uv4 = dst_mesh->getUV4();
    ASSERT_EQ(src_uv4.size(), dst_uv4.size()) << "alignしてもUV4の数は同じ";
    for(int i=0; i<src_uv4.size(); i++) {
        ASSERT_EQ(dst_uv4.at(i), src_uv4.at(i)) << "alignしてもUV4の値は変わらない";
    }

    // SubMeshチェック
    auto dst_sub_meshes = dst_mesh->getSubMeshes();
    ASSERT_EQ(src_sub_meshes.size(), dst_sub_meshes.size()) << "alignしてもSubMeshの数は同じ";
    for(int i=0; i < src_sub_meshes.size(); i++) {
        ASSERT_EQ(dst_sub_meshes.at(i).getStartIndex(), src_sub_meshes.at(i).getStartIndex()) << "alignしてもSubMeshのStartIndexは変わらない";
        ASSERT_EQ(dst_sub_meshes.at(i).getEndIndex(), src_sub_meshes.at(i).getEndIndex()) << "alignしてもSubMeshのEndIndexは変わらない";
        ASSERT_EQ(dst_sub_meshes.at(i).getGameMaterialID(), src_sub_meshes.at(i).getGameMaterialID()) << "alignしてもSubMeshのgameMaterialIDは変わらない";
    }

}
