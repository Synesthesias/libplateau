using System;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.GeometryModel;
using PLATEAU.Interop;
using PLATEAU.IO;
using PLATEAU.Test.CityGML;

namespace PLATEAU.Test.GeometryModel
{
    [TestClass]
    public class MeshExtractorTests
    {

        // TODO 以下のコメントアウトしたテストを別の形に書き直す
        // [TestMethod]
        // public void GridMerge_Returns_Polygons()
        // {
        //     var polygons = LoadAndGridMerge();
        //     foreach (var poly in polygons)
        //     {
        //         Console.WriteLine($"{poly.ID} : vertCount={poly.VertexCount}");
        //     }
        //     int numGridsWithVertices = polygons.Count(poly => poly.VertexCount > 0);
        //     Assert.IsTrue(numGridsWithVertices >= 7, "頂点を含むメッシュが7個以上ある");
        // }
        //
        // [TestMethod]
        // public void GridMerge_Composes_MultiTexture()
        // {
        //     var polygons = LoadAndGridMerge();
        //     var multiTextures = polygons
        //         .Select(poly => poly.GetMultiTexture())
        //         .Where(multiTex => multiTex.Length > 0)
        //         .ToArray();
        //     Assert.IsTrue(multiTextures.Length >= 3, "MultiTextureを含むポリゴンが3つ以上あります");
        //     Assert.IsFalse(string.IsNullOrEmpty(multiTextures[0][0].Texture.Url), "MultiTextureのURLにアクセスできます");
        // }
        //
        // [TestMethod]
        // public void When_GridMerge_Then_Size_Of_UV1_Equals_Num_Of_Vertices()
        // {
        //     AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv1());
        // }
        //
        // [TestMethod]
        // public void When_GridMerge_Then_Size_Of_UV2_Equals_Num_Of_Vertices()
        // {
        //     AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv2());
        // }
        //
        // [TestMethod]
        // public void When_GridMerge_Then_Size_Of_UV3_Equals_Num_Of_Vertices()
        // {
        //     AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv3());
        // }

        [TestMethod]
        public void Extract_Returns_Model_With_Nodes_With_Mesh()
        {
            using var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            using var meshExtractor = new MeshExtractor();
            var options = new MeshExtractOptions
            {
                ReferencePoint = cityModel.CenterPoint,
                MeshAxes = AxesConversion.WUN,
                MeshGranularity = MeshGranularity.PerCityModelArea,
                MinLod = 2,
                MaxLod = 2,
                ExportAppearance = true,
                GridCountOfSide = 5,
                UnitScale = 1f
            };
            
            // TODO 下記でいろいろテストしているので分けた方が良い
            var model = meshExtractor.Extract(cityModel, options);
            Assert.IsTrue(model.RootNodesCount > 0, "モデル内にルートノードが存在する");
            var rootNode = model.GetRootNodeAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(rootNode.Name), "ルートノードの1つを取得でき、その名前を取得できる");
            Assert.IsTrue(rootNode.ChildCount > 0, "ルートノードの子を取得できる");
            var firstChild = rootNode.GetChildAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(firstChild.Name), "子ノードの名前を取得できる");
            // メッシュを含むノードを検索
            bool doMeshExist = false;
            Mesh foundMesh = null;
            for (int i = 0; i < rootNode.ChildCount; i++)
            {
                var child = rootNode.GetChildAt(i);
                var mesh = child.Mesh;
                if (mesh == null) continue;
                if (mesh.VerticesCount > 0 && mesh.IndicesCount >= 3)
                {
                    doMeshExist = true;
                    foundMesh = mesh;
                    break;
                }
            }
            Assert.IsTrue(doMeshExist, "頂点と面が1つ以上あるメッシュが存在する");
            Assert.IsTrue(foundMesh.GetVertexAt(0).IsNotZero(), "メッシュの頂点を取得できる");
            Assert.IsTrue(foundMesh.GetIndiceAt(0) >= 0, "メッシュのIndicesリストの要素を取得できる");
            Assert.IsNull(rootNode.Mesh, "メッシュがない状況ではMeshはnullを返す");
            Assert.IsTrue(foundMesh.SubMeshCount > 0, "メッシュにはサブメッシュが存在する");
            var subMesh = foundMesh.GetSubMeshAt(0);
            Assert.IsTrue(subMesh.StartIndex >= 0, "サブメッシュのStartIndexを取得できる");
            Assert.IsTrue(subMesh.EndIndex >= 0, "サブメッシュのEndIndexを取得できる");
            Assert.IsNotNull(subMesh.TexturePath, "サブメッシュのテクスチャパスを取得できる");
        }
        
        // TODO　以下のコメントアウトしたテストを別の形に書き直す
        // private static Mesh[] LoadAndGridMerge()
        // {
        //     var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        //     var logger = new DllLogger();
        //     logger.SetCallbacksToStdOut();
        //     var meshExtractor = new MeshExtractor();
        //     var polygons = meshExtractor.GridMerge(cityModel, CityObjectType.COT_All, 5, 5, logger);
        //     return polygons;
        // }
        //
        // private static void AssertSizeOfUvEqualsNumOfVertices(
        //     Func<Mesh, PlateauVector2f[]> polygonToUvGetter)
        // {
        //     var polygons = LoadAndGridMerge();
        //     foreach (var poly in polygons)
        //     {
        //         Assert.AreEqual(polygonToUvGetter(poly).Length, poly.VertexCount);
        //     }
        // }

    }
}