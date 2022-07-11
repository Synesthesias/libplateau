using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class CityModelTests
    {
        private readonly CityModel cityModel;

        public CityModelTests()
        {
            this.cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        }

        [TestMethod]
        public void GetCityObjectsByType_Can_Be_Used_To_Get_All_CityObjects()
        {
            var allCityObjects = this.cityModel.GetCityObjectsByType(CityObjectType.COT_All);
            Assert.AreEqual(548, allCityObjects.Length);
        }

        [TestMethod]
        public void GetCityObjectsByType_Returns_CityObjects_With_Specified_Type_Without_Overlord()
        {
            var floorType = CityObjectType.COT_FloorSurface;

            var allCityObjects = this.cityModel.GetCityObjectsByType(CityObjectType.COT_All);
            var floorCityObjects = this.cityModel.GetCityObjectsByType(floorType);

            foreach (var obj in allCityObjects)
            {
                if (floorCityObjects.Any(co => co.ID == obj.ID))
                {
                    Assert.AreEqual(floorType, obj.Type);
                }
                else
                {
                    Assert.AreNotEqual(floorType, obj.Type);
                }
            }
        }

        [TestMethod]
        public void GetCityObjectsByType_Works_With_Multiple_Type_Flags()
        {
            // 今回のテストデータには、 COT_Building は含まれますが COT_Door は含まれません。
            var typeFlags = CityObjectType.COT_Building | CityObjectType.COT_Door;
            var foundCityObjs = this.cityModel.GetCityObjectsByType(typeFlags);
            foreach (var cityObjs in foundCityObjs)
            {
                var type = cityObjs.Type;
                Assert.IsTrue(type == CityObjectType.COT_Building || type == CityObjectType.COT_Door, 
                    $"{nameof(CityModel.GetCityObjectsByType)} で得られるものはすべてタイプが指定のものとなります。");
            }
            Assert.IsTrue(foundCityObjs.Any(co => co.Type == CityObjectType.COT_Building),
                $"{nameof(CityObjectType.COT_Building)} が1つ以上見つかります。");
        }

        [TestMethod]
        public void GetCityObjectFromId_Returns_CityObj_With_Same_ID()
        {
            const string id = "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745";
            var found = this.cityModel.GetCityObjectById(id);
            Assert.AreEqual(id, found.ID);
        }

        [TestMethod]
        public void GetCityObjectFromId_Throws_Exception_When_Not_Found()
        {
            Assert.ThrowsException<KeyNotFoundException>(() =>
            {
                this.cityModel.GetCityObjectById("存在しないID-ダミー012");
            });
        }
    }
}