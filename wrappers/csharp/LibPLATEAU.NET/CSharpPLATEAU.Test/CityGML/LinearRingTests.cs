using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class LinearRingTests
    {
        private readonly LinearRing exteriorRing;
        
        // 前処理
        public LinearRingTests()
        {
            CityModel cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, false);
            
            // 頂点数が 1 以上である ExteriorRing を検索し、最初に見つかったものをテスト対象とします。
            this.exteriorRing = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.Polygons)
                .First(poly => poly.ExteriorRing.VertexCount > 0)
                .ExteriorRing;
        }
        
        [TestMethod]
        public void ExteriorRing_Is_Not_Null()
        {
            Console.WriteLine(this.exteriorRing.Handle.ToInt64());
            Assert.IsNotNull(this.exteriorRing);
        }
        
        [TestMethod]
        public void VerticesCount_Returns_Positive_Number()
        {
            int verticesCount = this.exteriorRing.VertexCount;
            Console.WriteLine($"Vertices Count: {verticesCount}");
            Assert.IsTrue(verticesCount > 0);
        }

        [TestMethod]
        public void GetVertex_Returns_Non_Zero()
        {
            var vert = this.exteriorRing.GetVertex(this.exteriorRing.VertexCount - 1);
            Console.WriteLine($"Vertex: {vert}");
            Assert.IsTrue(vert.IsNotZero());
        }
    }
}