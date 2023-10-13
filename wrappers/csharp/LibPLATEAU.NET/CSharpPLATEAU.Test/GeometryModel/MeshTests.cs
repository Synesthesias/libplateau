using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geometries;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;
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
            int numUV4 = foundMesh.GetUv4().Length;
            Assert.AreEqual(numVert, numUV1);
            Assert.AreEqual(numVert, numUV1);
            Assert.AreEqual(numVert, numUV4);
        }

        [TestMethod]
        public void CreateMesh()
        {
            var mesh = Mesh.Create("testMesh");
            Assert.AreEqual(0, mesh.VerticesCount);
            mesh.Dispose();
        }

        [TestMethod]
        public void Merge_Same_Mesh_Then_Vertex_Count_Doubles()
        {
            var model = TestGeometryUtil.ExtractModel();
            var mesh = TestGeometryUtil.FirstMeshInModel(model);
            // 同じインスタンス同士でマージするとバグになるので
            // 実質的なコピーを用意するためもう一度ロードします。
            var model2 = TestGeometryUtil.ExtractModel();
            var mesh2 = TestGeometryUtil.FirstMeshInModel(model2);
            
            int numVert = mesh.VerticesCount;
            int numUV1 = mesh.GetUv1().Length;
            int numIndices = mesh.IndicesCount;
            mesh.MergeMesh(mesh2, true);
            Assert.AreEqual(2 * numVert, mesh.VerticesCount);
            Assert.AreEqual(2 * numUV1, mesh.GetUv1().Length);
            Assert.AreEqual(2 * numIndices, mesh.IndicesCount);
        }

        [TestMethod]
        public void MergeMeshData_From_Empty_Mesh()
        {
            var mesh = CreateSimpleMesh();
            Assert.AreEqual(3, mesh.GetUv1().Length);
            Assert.AreEqual(33, mesh.GetVertexAt(2).Z);
            Assert.AreEqual(2, mesh.GetIndiceAt(2));
            Console.WriteLine($"{mesh.GetUv1()[0]}, {mesh.GetUv1()[1]}, {mesh.GetUv1()[2]}");
            Assert.AreEqual(3.2f, mesh.GetUv1()[2].Y);
            
        }

        [TestMethod]
        public void SetAndGetCityObjectList()
        {
            var mesh = CreateSimpleMesh();
            using (var cityObjListToAdd = CityObjectList.Create())
            {
                cityObjListToAdd.Add(new CityObjectIndex(0,0), "gml-id");
                mesh.CityObjectList = cityObjListToAdd;
            }

            var cityObjList = mesh.CityObjectList;
            bool found = cityObjList.TryGetAtomicID(new CityObjectIndex(0, 0), out var gmlID);
            Assert.IsTrue(found);
            Assert.AreEqual("gml-id", gmlID);
        }

        [TestMethod]
        public void AddSubMesh()
        {
            var mesh = CreateSimpleMesh();
            mesh.AddSubMesh("test.png", 0, 2);
            var subMesh = mesh.GetSubMeshAt(1);
            Assert.AreEqual("test.png", subMesh.TexturePath);
        }

        private static Mesh CreateSimpleMesh()
        {
            SimpleMeshInfo(out var vertices, out var indices, out var uv1, out var uv4, out var subMeshes);
            var mesh = Mesh.Create(vertices, indices, uv1, uv4, subMeshes);
            return mesh;
        }

        private static void SimpleMeshInfo(out PlateauVector3d[] vertices, out uint[] indices,
            out PlateauVector2f[] uv1, out PlateauVector2f[] uv4, out SubMesh[] subMeshes)
        {
            vertices = new[]
            {
                new PlateauVector3d(11, 12, 13),
                new PlateauVector3d(21, 22, 23),
                new PlateauVector3d(31, 32, 33)
            };
            indices = new uint[]
            {
                0, 1, 2
            };
            uv1 = new[]
            {
                new PlateauVector2f(1.1f, 1.2f),
                new PlateauVector2f(2.1f, 2.2f),
                new PlateauVector2f(3.1f, 3.2f)
            };
            uv4 = new[]
            {
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0)
            };
            subMeshes = new[]
            {
                SubMesh.Create(0, 2, "testTexturePath.png")
            };
        }
        
    }
}
