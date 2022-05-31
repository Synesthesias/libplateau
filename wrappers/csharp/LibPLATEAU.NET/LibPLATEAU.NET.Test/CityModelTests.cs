using System.Collections.Generic;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
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
        public void GetCityObjectFromId_Returns_CityObj_With_Same_ID()
        {
            const string id = "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745";
            var found = this.cityModel.GetCityObjectFromId(id);
            Assert.AreEqual(id, found.ID);
        }

        [TestMethod]
        public void GetCityObjectFromId_Throws_Exception_When_Not_Found()
        {
            Assert.ThrowsException<KeyNotFoundException>(() =>
            {
                this.cityModel.GetCityObjectFromId("存在しないID-ダミー012");
            });
        }
    }
}