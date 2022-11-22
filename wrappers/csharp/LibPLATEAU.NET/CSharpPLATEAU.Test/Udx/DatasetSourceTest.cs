using System;
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
            Assert.AreEqual(accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building).Get(0).MeshCode.ToString(), "53392642");
        }
    }
}
