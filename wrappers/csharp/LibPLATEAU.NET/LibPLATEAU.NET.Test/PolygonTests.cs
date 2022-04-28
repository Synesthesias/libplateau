using System;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class PolygonTests
    {
        private Polygon polygon;
        // 前処理
        public PolygonTests()
        {
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile();
            CityObject cityObject = cityModel.RootCityObjects[0];
            Geometry geometry = cityObject.GetGeometry(0);
            // Polygon を有する Geometry であれば何でもよいのでそれを指定
            Geometry childGeom = geometry.GetChildGeometry(0);
            this.polygon = childGeom.GetPolygon(0);
        }

        [TestMethod]
        public void VertexCount_Returns_Positive_Value()
        {
            int actualVertCount = this.polygon.VertexCount;
            Console.WriteLine($"VertexCount: {actualVertCount}");
            Assert.IsTrue(actualVertCount > 0);
        }
        

        [TestMethod]
        public void GetVertex_Returns_Non_Zero_Position()
        {
            var vert = this.polygon.GetVertex(0);
            Console.WriteLine($"vertex: {vert}");
            Assert.IsTrue(Math.Abs(vert.X) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Y) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Z) > 0.001);
        }

        [TestMethod]
        public void IndicesCount_Returns_Positive_Value()
        {
            int indicesCount = this.polygon.IndicesCount;
            Console.WriteLine($"indices count: {indicesCount}");
            Assert.IsTrue(indicesCount > 0);
        }
    }
}