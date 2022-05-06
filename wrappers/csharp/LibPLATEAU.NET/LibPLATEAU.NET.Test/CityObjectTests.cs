using System;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class CityObjectTests
    {
        private readonly CityObject cityObject;

        public CityObjectTests() {
            // Load処理が重いため最初のみ実行して使いまわす。
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile();
            this.cityObject = cityModel.RootCityObjects[0];
        }

        [TestMethod]
        public void ID_Returns_GML_ID()
        {
            Assert.AreEqual(
                "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745",
                this.cityObject.ID);
        }

        [TestMethod]
        public void Type_Returns_GML_Type()
        {
            Assert.AreEqual(
                CityObjectType.COT_Building,
                this.cityObject.Type);
        }

        [TestMethod]
        public void GeometryCount_Returns_Positive_Value() {
            int geomCount = this.cityObject.GeometryCount;
            Assert.IsTrue(geomCount > 0);
            
            Console.WriteLine($"Geometries Count: {geomCount}");
        }

        [TestMethod]
        public void ImplicitGeometryCount_Returns_Positive_Value_Or_Zero()
        {
            int actualCount = this.cityObject.ImplicitGeometryCount;

            Assert.IsTrue(actualCount >= 0);
            
            Console.WriteLine($"Implicit Geometry Count: {actualCount}");
        }

        [TestMethod]
        public void ChildCityObjectCount_Returns_Positive_Value()
        {
            int actualCount = this.cityObject.ChildCityObjectCount;
            Console.WriteLine($"Child City Object Count : {actualCount}");
            Assert.IsTrue(actualCount > 0);
        }

        [TestMethod]
        public void GetChildCityObject_Returns_GML_Child()
        {
            for (int i = 0; i < this.cityObject.ChildCityObjectCount; i++)
            {
                var co = this.cityObject.GetChildCityObject(i);
                Console.WriteLine($"Child {i}: Type {co.Type} , ID {co.ID}");
            }

            var child = this.cityObject.GetChildCityObject(0);
            const CityObjectType gmlChildType = CityObjectType.COT_GroundSurface;
            const string gmlChildId = "gnd_9f9d1116-eb56-47fa-bfc4-7c0df1426dd9";
            CityObjectType actualType = child.Type;
            string actualId = child.ID;
            
            Assert.AreEqual(gmlChildType, actualType);
            Assert.AreEqual(gmlChildId, actualId);

        }

        [TestMethod]
        public void Cache_Of_GetChildCityObject_Returns_Same_As_Original()
        {
            var original = this.cityObject.GetChildCityObject(0);
            var cached = this.cityObject.GetChildCityObject(0);
            Assert.AreEqual(original, cached);
        }

        [TestMethod]
        public void GetGeometry_Returns_GML_Geometry()
        {
            int count = this.cityObject.GeometryCount;
            // 参考用にGeometry一覧を出力
            for (int i = 0; i < count; i++)
            {
                var geom = this.cityObject.GetGeometry(i);
                Console.WriteLine($"{i}: {geom}");
            }

            GeometryType gmlType = GeometryType.GT_Roof;
            GeometryType actualType = this.cityObject.GetGeometry(0).Type;
            Assert.AreEqual(gmlType, actualType);
        }
    }
}