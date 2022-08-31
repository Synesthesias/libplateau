using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Test.CityGML;

namespace PLATEAU.Test.GeometryModel
{
    [TestClass]
    public class MeshTests
    {
        [TestMethod]
        public void Mesh_Can_Get_Vertices_And_Indices_And_SubMeshes()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            Assert.IsTrue(foundMesh.GetVertexAt(0).IsNotZero(), "メッシュの頂点を取得できる");
            Assert.IsTrue(foundMesh.GetIndiceAt(0) >= 0, "メッシュのIndicesリストの要素を取得できる");
            Assert.IsTrue(foundMesh.SubMeshCount > 0, "メッシュにはサブメッシュが存在する");
            var subMesh = foundMesh.GetSubMeshAt(0);
            Assert.IsTrue(subMesh.StartIndex >= 0, "サブメッシュのStartIndexを取得できる");
            Assert.IsTrue(subMesh.EndIndex >= 0, "サブメッシュのEndIndexを取得できる");
            Assert.IsNotNull(subMesh.TexturePath, "サブメッシュのテクスチャパスを取得できる");
        }

        [TestMethod]
        public void Throws_IndexOutOfRangeException_When_Index_Is_OutOfRange()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            Assert.ThrowsException<IndexOutOfRangeException>(() => foundMesh.GetVertexAt(999999999), "GetVertexAtで範囲外アクセスの時に例外が出る");
            Assert.ThrowsException<IndexOutOfRangeException>(() => foundMesh.GetSubMeshAt(999999999));
        }

        [TestMethod]
        public void Size_Of_UV_Equals_Num_Of_Vertices()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            int numVert = foundMesh.VerticesCount;
            int numUV1 = foundMesh.GetUv1().Length;
            int numUV2 = foundMesh.GetUv2().Length;
            int numUV3 = foundMesh.GetUv3().Length;
            Assert.AreEqual(numVert, numUV1);
            Assert.AreEqual(numVert, numUV2);
            Assert.AreEqual(numVert, numUV3);
        }
    }
}
