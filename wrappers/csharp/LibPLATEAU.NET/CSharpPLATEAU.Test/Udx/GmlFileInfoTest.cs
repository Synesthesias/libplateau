using System.IO;
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
        
        [TestMethod]
        public void Fetch_Copies_Relative_Files()
        {
            var collection = LocalDatasetAccessor.Find("data");
            var testDir = Directory.CreateDirectory("temp_test_dir");
            var gmlArray = collection.GetGmlFiles(PredefinedCityModelPackage.Building);
            foreach (var gml in gmlArray)
            {
                GmlFileInfo.Create(gml).Fetch(testDir.FullName);
            }

            var shouldExists = new[]
            {
                "udx/bldg/53392642_bldg_6697_op2.gml",
                "udx/bldg/53392642_bldg_6697_appearance/hnap0034.tif",
                "codelists/Common_prefecture.xml"
            };
            foreach (var filePath in shouldExists)
            {
                var resultPath = "temp_test_dir/data/" + filePath;
                Assert.IsTrue(File.Exists(resultPath), $"{resultPath} does not exist");
            }
            Directory.Delete(testDir.FullName, true);
        }
    }
}
