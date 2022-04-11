using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class CityObjectTests
    {
        private readonly CityObject cityObject;

        public CityObjectTests() {
            // Load�������d�����ߍŏ��̂ݎ��s���Ďg���܂킷�B
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile();
            cityObject = cityModel.RootCityObjects[0];
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
    }
}