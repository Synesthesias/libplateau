using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class GmlFileInfoTest
    {
        [TestMethod]
        public void GetPath_Returns_Path()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            using var info = GmlFileInfo.Create(path);
            Assert.AreEqual(path, info.Path);
        }

        [TestMethod]
        public void FeatureType()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            using var info = GmlFileInfo.Create(path);
            Assert.AreEqual("bldg", info.FeatureType);
        }
        
        [TestMethod]
        public void DirNameToPackage_Returns_Gml_Package()
        {
            using var gmlInfo = GmlFileInfo.Create("foobar/udx/bldg/53392546_bldg_6697_2_op.gml");
            Assert.AreEqual(PredefinedCityModelPackage.Building, gmlInfo.Package);
        }
    }
}
