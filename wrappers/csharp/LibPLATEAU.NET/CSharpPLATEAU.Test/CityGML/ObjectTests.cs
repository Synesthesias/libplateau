using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using Object = PLATEAU.CityGML.Object;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class ObjectTests
    {
        private static CityModel cityModel;
        private static Object plateauObject;

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            plateauObject = cityModel.RootCityObjects[0];
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void ID_Returns_GML_ID()
        {
            const string idInGmlFile = "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745";
            string actualId = plateauObject.ID;
            Assert.AreEqual(idInGmlFile, actualId);
        }
    }
}
