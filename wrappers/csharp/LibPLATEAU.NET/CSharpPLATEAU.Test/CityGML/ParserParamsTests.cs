using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class ParserParamsTests
    {
        [TestMethod]
        public void When_IgnoreGeometries_Is_False_Then_Geometry_Exists()
        {
            using var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var allCityObjects = cityModel.RootCityObjects.SelectMany(co => co.CityObjectDescendantsDFS);
            var geom = allCityObjects.SelectMany(co => co.Geometries);
            Assert.IsTrue(geom.Any());
        }

        [TestMethod]
        public void When_IgnoreGeometries_Is_True_Then_Geometry_Dont_Exist()
        {
            using var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, true, true);
            var allCityObjects = cityModel.RootCityObjects.SelectMany(co => co.CityObjectDescendantsDFS);
            var geom = allCityObjects.SelectMany(co => co.Geometries);
            Assert.IsFalse(geom.Any());
        }
    }
}
