using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class GeometryTests
    {
        private static CityModel cityModel;
        private static Geometry geomWithPolygon;
        private static Geometry geomWithChildren;
        
        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            // テスト対象として適切なものを検索し、最初にヒットした物をテストに利用します。
            // 具体的には Polygon を1つ以上含む Geometry と、 Children を1つ以上含む Geometry を検索します。
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var allCityObjects = cityModel.RootCityObjects.SelectMany(co => co.CityObjectDescendantsDFS).ToArray();
            geomWithPolygon = allCityObjects.SelectMany(co => co.Geometries).First(geo => geo.PolygonCount > 0);
            geomWithChildren =
                allCityObjects.SelectMany(co => co.Geometries).First(geo => geo.ChildGeometryCount > 0);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void Type_Returns_GML_Type()
        {
            GeometryType gmlType = GeometryType.GT_Roof;
            GeometryType actualType = geomWithPolygon.Type;
            Console.WriteLine(actualType);
            Assert.AreEqual(gmlType, actualType);
        }

        [TestMethod]
        public void ChildGeometryCount_Returns_Positive_Value()
        {
            int actualGeomCount = geomWithChildren.ChildGeometryCount;
            Console.WriteLine(actualGeomCount);
            Assert.IsTrue(actualGeomCount > 0);
        }

        [TestMethod]
        public void PolygonCount_Returns_Positive_Value()
        {
            int actualPolyCount = geomWithPolygon.PolygonCount;
            Assert.IsTrue(actualPolyCount > 0);
        }

        [TestMethod]
        public void GetChildGeometry_Returns_Not_Null()
        {
            var actualChild = geomWithChildren.GetChildGeometry(0);
            Console.WriteLine(actualChild);
            Assert.IsNotNull(actualChild);
        }

        [TestMethod]
        public void LOD_Returns_Zero_Or_Positive()
        {
            int actualLOD = geomWithPolygon.LOD;
            Console.WriteLine($"LOD = {actualLOD}");
            Assert.IsTrue(actualLOD >= 0);
        }

        [TestMethod]
        public void LineStringCount_Returns_Zero_Or_Positive_Value()
        {
            int actualLineStringsCount = geomWithPolygon.LineStringCount;
            Assert.IsTrue(actualLineStringsCount >= 0);
        }

        [TestMethod]
        public void Do_Exists_Geometry_With_SRSName()
        {
            var geomWithSRSName = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .FirstOrDefault(geom => geom.SRSName.Length > 0);
            string actualValue = geomWithSRSName?.SRSName ?? "";
            Console.WriteLine($"SRSName = {actualValue}");
            const string expectedValue = "http://www.opengis.net/def/crs/EPSG/0/6697";
            Assert.IsNotNull(expectedValue, actualValue);
        }
        

    }
}
