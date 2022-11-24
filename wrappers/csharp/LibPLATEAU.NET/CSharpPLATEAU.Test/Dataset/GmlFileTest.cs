using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Interop;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class GmlFileTest
    {
        [TestMethod]
        public void GetPath_Returns_Path()
        {
            const string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual(path, info.Path);
            info.Dispose();
        }

        [TestMethod]
        public void FeatureType()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual("bldg", info.FeatureType);
            info.Dispose();
        }

        [TestMethod]
        public void TestMeshCode()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual("53392642", info.MeshCode.ToString());
            info.Dispose();
        }
        
        [TestMethod]
        public void DirNameToPackage_Returns_Gml_Package()
        {
            var gmlInfo = GmlFile.Create("foobar/udx/bldg/53392546_bldg_6697_2_op.gml");
            Assert.AreEqual(PredefinedCityModelPackage.Building, gmlInfo.Package);
            gmlInfo.Dispose();
        }
        
        [TestMethod]
        public void Fetch_Copies_Relative_Files()
        {
            using var source = DatasetSource.CreateLocal("data");
            var accessor = source.Accessor;
            var testDir = Directory.CreateDirectory("temp_test_dir");
            var gmls = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            foreach (var gml in gmls)
            {
                gml.Fetch(testDir.FullName);
            }

            string[] shouldExists = 
            {
                "udx/bldg/53392642_bldg_6697_op2.gml",
                "udx/bldg/53392642_bldg_6697_appearance/hnap0034.tif",
                "codelists/Common_prefecture.xml"
            };
            foreach (string filePath in shouldExists)
            {
                string resultPath = "temp_test_dir/data/" + filePath;
                Assert.IsTrue(File.Exists(resultPath), $"{resultPath} does not exist");
            }
            Directory.Delete(testDir.FullName, true);
        }
    }
}
