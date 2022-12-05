using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class LinearRingTests
    {
        private static CityModel cityModel;
        private static LinearRing exteriorRing;

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, false);
            
            // 頂点数が 1 以上である ExteriorRing を検索し、最初に見つかったものをテスト対象とします。
            exteriorRing = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.Polygons)
                .First(poly => poly.ExteriorRing.VertexCount > 0)
                .ExteriorRing;
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }
        
        [TestMethod]
        public void ExteriorRing_Is_Not_Null()
        {
            Console.WriteLine(exteriorRing.Handle.ToInt64());
            Assert.IsNotNull(exteriorRing);
        }
        
        [TestMethod]
        public void VerticesCount_Returns_Positive_Number()
        {
            int verticesCount = exteriorRing.VertexCount;
            Console.WriteLine($"Vertices Count: {verticesCount}");
            Assert.IsTrue(verticesCount > 0);
        }

        [TestMethod]
        public void GetVertex_Returns_Non_Zero()
        {
            var vert = exteriorRing.GetVertex(exteriorRing.VertexCount - 1);
            Console.WriteLine($"Vertex: {vert}");
            Assert.IsTrue(vert.IsNotZero());
        }
    }
}
