using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Network;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class DatasetSourceTest
    {
        [TestMethod]
        public void Get_Accessor_From_Local_DataSource_Returns_Accessor()
        {
            using var source = DatasetSource.Create(false, "data/日本語パステスト", "");
            using var accessor = source.Accessor;
            Assert.AreEqual(accessor.GetGmlFiles(PredefinedCityModelPackage.Building).At(0).MeshCode.ToString(), "53392642");
        }

        [TestMethod]
        public void Accessor_From_Server_Works()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
            Assert.AreEqual("53392670", accessor.MeshCodes.At(1).ToString());
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(
                NetworkConfig.DefaultApiServerUrl + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml",
                gmls.At(0).Path);
            var expectedPackages =
                PredefinedCityModelPackage.Building | PredefinedCityModelPackage.Road |
                PredefinedCityModelPackage.LandUse |
                PredefinedCityModelPackage.UrbanPlanningDecision;
            Assert.AreEqual(expectedPackages, accessor.Packages);
        }
    }
}
