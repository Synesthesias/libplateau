using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class DatasetSourceTest
    {
        [TestMethod]
        public void Get_Accessor_From_Local_DataSource_Returns_Accessor()
        {
            using var source = DatasetSource.CreateLocal("data");
            var accessor = source.Accessor;
            Assert.AreEqual(accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building).At(0).MeshCode.ToString(), "53392642");
        }

        [TestMethod]
        public void Accessor_From_Local_Source_Can_Search_MaxLod()
        {
            using var source = DatasetSource.CreateLocal("data");
            var accessor = source.Accessor;
            var meshCode = accessor.MeshCodes.At(0);
            var package = PredefinedCityModelPackage.Building;
            Assert.AreEqual(2, accessor.GetMaxLod(meshCode, package));
        }
    }
}
