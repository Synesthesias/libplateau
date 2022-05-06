
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
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            plateauObject = cityModel.RootCityObjects[0];
        }

        [TestMethod]
        public void ID_Returns_GML_ID()
        {
            const string idInGmlFile = "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745";
            string actualId = this.plateauObject.ID;
            Assert.AreEqual(idInGmlFile, actualId);
        }
    }
}