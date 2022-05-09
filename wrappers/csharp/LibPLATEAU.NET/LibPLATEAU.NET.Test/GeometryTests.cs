using System;
using System.Diagnostics.Contracts;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using LibPLATEAU.NET.Util;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class GeometryTests
    {
        private readonly Geometry geomWithPolygon;

        private readonly Geometry geomWithChildren;
        // 初期化
        public GeometryTests()
        {
            // テスト対象として適切なものを検索し、最初にヒットした物をテストに利用します。
            // 具体的には Polygon を1つ以上含む Geometry と、 Children を1つ以上含む Geometry を検索します。
            CityModel cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var allCityObjects = cityModel.RootCityObjects.SelectMany(co => co.CityObjectDescendantsDFS).ToArray();
            this.geomWithPolygon = allCityObjects.SelectMany(co => co.Geometries).First(geo => geo.PolygonCount > 0);
            this.geomWithChildren =
                allCityObjects.SelectMany(co => co.Geometries).First(geo => geo.ChildGeometryCount > 0);
        }

        [TestMethod]
        public void Type_Returns_GML_Type()
        {
            GeometryType gmlType = GeometryType.GT_Roof;
            GeometryType actualType = this.geomWithPolygon.Type;
            Console.WriteLine(actualType);
            Assert.AreEqual(gmlType, actualType);
        }

        [TestMethod]
        public void ChildGeometryCount_Returns_Positive_Value()
        {
            int actualGeomCount = this.geomWithChildren.ChildGeometryCount;
            Console.WriteLine(actualGeomCount);
            Assert.IsTrue(actualGeomCount > 0);
        }

        [TestMethod]
        public void PolygonCount_Returns_Positive_Value()
        {
            int actualPolyCount = this.geomWithPolygon.PolygonCount;
            Assert.IsTrue(actualPolyCount > 0);
        }

        [TestMethod]
        public void GetChildGeometry_Returns_Not_Null()
        {
            var actualChild = this.geomWithChildren.GetChildGeometry(0);
            Console.WriteLine(actualChild);
            Assert.IsNotNull(actualChild);
        }

        [TestMethod]
        public void LOD_Returns_Zero_Or_Positive()
        {
            int actualLOD = this.geomWithPolygon.LOD;
            Console.WriteLine($"LOD = {actualLOD}");
            Assert.IsTrue(actualLOD >= 0);
        }

        [TestMethod]
        public void LineStringCount_Returns_Zero_Or_Positive_Value()
        {
            int actualLineStringsCount = this.geomWithPolygon.LineStringCount;
            Assert.IsTrue(actualLineStringsCount >= 0);
        }
        

    }
}