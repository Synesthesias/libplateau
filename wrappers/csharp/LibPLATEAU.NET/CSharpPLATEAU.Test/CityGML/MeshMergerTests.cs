using System;
using System.Diagnostics;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.Interop;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class MeshMergerTests
    {
        private readonly MeshMerger meshMerger;

        public MeshMergerTests()
        {
            this.meshMerger = new MeshMerger();
        }

        [TestMethod]
        public void GridMerge_Returns_Polygons()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            var polygons = this.meshMerger.GridMerge(cityModel, CityObjectType.COT_All, 5, 5, logger);
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
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            var polygons = this.meshMerger.GridMerge(cityModel, CityObjectType.COT_All, 5, 5, logger);
            var multiTextures = polygons
                .Select(poly => poly.GetMultiTexture())
                .Where(multiTex => multiTex.Length > 0)
                .ToArray();
            Assert.IsTrue(multiTextures.Length >= 3, "MultiTextureを含むポリゴンが3つ以上あります");
            Assert.IsFalse(string.IsNullOrEmpty(multiTextures[0][0].Texture.Url), "MultiTextureのURLにアクセスできる");
    }

    }
}