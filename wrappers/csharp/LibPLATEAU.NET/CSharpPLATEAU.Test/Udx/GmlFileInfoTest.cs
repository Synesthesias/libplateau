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
            var info = GmlFileInfo.Create(path);
            Assert.AreEqual(path, info.Path);
        }
    }
}
