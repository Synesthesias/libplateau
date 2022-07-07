using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.IO;

namespace PLATEAU.Test.IO
{
    [TestClass]
    public class PrimaryCityObjectTypesTest
    {
        [TestMethod]
        public void IsPrimary_Checks_If_CityObject_Is_Primary()
        {
            Assert.IsTrue(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_Building));
            Assert.IsTrue(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_BuildingPart));
            Assert.IsTrue(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_Road));
            Assert.IsTrue(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_Bridge | CityObjectType.COT_LandUse));

            Assert.IsFalse(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_RoofSurface));
            Assert.IsFalse(PrimaryCityObjectTypes.IsPrimary(CityObjectType.COT_Door | CityObjectType.COT_Window));
        }
    }
}
