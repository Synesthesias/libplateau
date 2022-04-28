using System;
using System.Diagnostics.Contracts;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class GeometryTests
    {
        private Geometry geom;
        // 初期化
        public GeometryTests()
        {
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile();
            CityObject cityObject = cityModel.RootCityObjects[0];
            this.geom = cityObject.GetGeometry(0);
        }

        [TestMethod]
        public void Type_Returns_GML_Type()
        {
            GeometryType gmlType = GeometryType.GT_Unknown;
            GeometryType actualType = this.geom.Type;
            Console.WriteLine(actualType);
            Assert.AreEqual(gmlType, actualType);
        }

        [TestMethod]
        public void ChildGeometryCount_Returns_Positive_Value()
        {
            int actualGeomCount = this.geom.ChildGeometryCount;
            Console.WriteLine(actualGeomCount);
            Assert.IsTrue(actualGeomCount > 0);
        }

        [TestMethod]
        public void PolygonCount_Returns_Positive_Value()
        {
            var child = this.geom.GetChildGeometry(0);
            int actualPolyCount = child.PolygonCount;
            Assert.IsTrue(actualPolyCount > 0);
        }

        [TestMethod]
        public void GetChildGeometry_Returns_Not_Null()
        {
            var actualChild = this.geom.GetChildGeometry(0);
            Console.WriteLine(actualChild);
            Assert.IsNotNull(actualChild);
        }

        [TestMethod]
        public void LOD_Returns_Zero_Or_Positive()
        {
            int actualLOD = this.geom.LOD;
            Console.WriteLine($"LOD = {actualLOD}");
            Assert.IsTrue(actualLOD >= 0);
        }

        [TestMethod]
        public void LineStringCount_Returns_Zero_Or_Positive_Value()
        {
            int actualLineStringsCount = this.geom.LineStringCount;
            Assert.IsTrue(actualLineStringsCount >= 0);
        }
        

    }
}