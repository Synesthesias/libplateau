﻿using System;
using System.Linq;
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
                ReferencePoint = new PlateauVector3d(0, 0, 0),
                MeshAxes = AxesConversion.WUN,
                MeshGranularity = MeshGranularity.PerCityModelArea,
                MinLod = 2,
                MaxLod = 2,
                ExportAppearance = true,
                GridCountOfSide = 5
            };
            var model = meshExtractor.Extract(cityModel, options, logger);
            Assert.IsTrue(model.RootNodesCount > 0, "モデル内にルートノードが存在する");
            var firstNode = model.GetRootNodeAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(firstNode.Name), "ルートノードの1つを取得でき、その名前を取得できる");
            Assert.IsTrue(firstNode.ChildCount > 0, "ルートノードの子を取得できる");
            var firstChild = firstNode.GetChildAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(firstChild.Name), "子ノードの名前を取得できる");
            var firstMesh = firstChild.Mesh;
            Assert.IsFalse(string.IsNullOrEmpty(firstMesh.ID), "メッシュを取得できる");
            Assert.IsNull(firstNode.Mesh, "メッシュがない状況ではMeshはnullを返す");
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