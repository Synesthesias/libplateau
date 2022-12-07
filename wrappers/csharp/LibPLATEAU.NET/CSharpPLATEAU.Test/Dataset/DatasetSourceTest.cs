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
            using var source = DatasetSource.Create(false, "data");
            var accessor = source.Accessor;
            Assert.AreEqual(accessor.GetGmlFiles(PredefinedCityModelPackage.Building).At(0).MeshCode.ToString(), "53392642");
        }

        [TestMethod]
        public void Accessor_From_Local_Source_Can_Search_MaxLod()
        {
            using var source = DatasetSource.Create(false, "data");
            var accessor = source.Accessor;
            var meshCode = accessor.MeshCodes.At(0);
            var package = PredefinedCityModelPackage.Building;
            Assert.AreEqual(2, accessor.GetMaxLod(meshCode, package));
        }

        [TestMethod]
        public void Accessor_From_Server_Works()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            Assert.AreEqual("53392642", accessor.MeshCodes.At(0).ToString());
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
