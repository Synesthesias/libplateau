using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
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
            using var accessor = source.Accessor;
            Assert.AreEqual(accessor.MeshCodes[0].ToString(), "53392642");
        }
    }
}
