using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.Interop;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class MeshMergerTests
    {

        [TestMethod]
        public void GridMerge_Returns_Polygons()
        {
            var polygons = LoadAndGridMerge();
            foreach (var poly in polygons)
            {
                Console.WriteLine($"{poly.ID} : vertCount={poly.VertexCount}");
            }
            int numGridsWithVertices = polygons.Count(poly => poly.VertexCount > 0);
            Assert.IsTrue(numGridsWithVertices >= 7, "頂点を含むメッシュが7個以上ある");
        }

        [TestMethod]
        public void GridMerge_Composes_MultiTexture()
        {
            var polygons = LoadAndGridMerge();
            var multiTextures = polygons
                .Select(poly => poly.GetMultiTexture())
                .Where(multiTex => multiTex.Length > 0)
                .ToArray();
            Assert.IsTrue(multiTextures.Length >= 3, "MultiTextureを含むポリゴンが3つ以上あります");
            Assert.IsFalse(string.IsNullOrEmpty(multiTextures[0][0].Texture.Url), "MultiTextureのURLにアクセスできます");
        }

        [TestMethod]
        public void When_GridMerge_Then_Size_Of_UV1_Equals_Num_Of_Vertices()
        {
            AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv1());
        }
        
        [TestMethod]
        public void When_GridMerge_Then_Size_Of_UV2_Equals_Num_Of_Vertices()
        {
            AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv2());
        }
        
        [TestMethod]
        public void When_GridMerge_Then_Size_Of_UV3_Equals_Num_Of_Vertices()
        {
            AssertSizeOfUvEqualsNumOfVertices(poly => poly.GetUv3());
        }

        private static Mesh[] LoadAndGridMerge()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            var meshMerger = new MeshExtractor();
            var polygons = meshMerger.GridMerge(cityModel, CityObjectType.COT_All, 5, 5, logger);
            return polygons;
        }

        private static void AssertSizeOfUvEqualsNumOfVertices(
            Func<Mesh, PlateauVector2f[]> polygonToUvGetter)
        {
            var polygons = LoadAndGridMerge();
            foreach (var poly in polygons)
            {
                Assert.AreEqual(polygonToUvGetter(poly).Length, poly.VertexCount);
            }
        }

    }
}