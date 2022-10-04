using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class CityModelPackageInfoTest
    {
        [TestMethod]
        public void Getter_Returns_Values_Of_Create()
        {
            using var info = CityModelPackageInfo.Create(true, 1, 2);
            Assert.AreEqual(true, info.HasAppearance);
            Assert.AreEqual(1, info.MinLOD);
            Assert.AreEqual(2, info.MaxLOD);
        }

        [TestMethod]
        public void Get_Predefined()
        {
            var buildingPredefined = CityModelPackageInfo.GetPredefined(PredefinedCityModelPackage.Building);
            Assert.AreEqual(true, buildingPredefined.hasAppearance);
            Assert.AreEqual(0, buildingPredefined.minLOD);
            Assert.AreEqual(3, buildingPredefined.maxLOD);
        }
    }
}
