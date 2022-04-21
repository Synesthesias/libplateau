using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Object = LibPLATEAU.NET.CityGML.Object;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjectTests
    {
        private Object plateauObject;

        public ObjectTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            plateauObject = cityModel.RootCityObjects[0];
        }

        [TestMethod]
        public void Test_Get_Id()
        {
            Assert.AreEqual(this.plateauObject.ID, "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745");
        }
    }
}