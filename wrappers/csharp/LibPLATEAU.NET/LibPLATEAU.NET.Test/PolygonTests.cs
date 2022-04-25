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
    }
}