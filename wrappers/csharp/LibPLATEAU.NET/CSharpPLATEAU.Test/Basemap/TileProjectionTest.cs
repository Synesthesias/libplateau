using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Basemap;

namespace PLATEAU.Test.Basemap
{
    [TestClass]
    public class TileProjectionTest
    {
        [TestMethod]
        public void X_Equals_UnProject_Project_X()
        {
            var tileCoord = new TileCoordinate(29107, 12908, 15);
            var extent = TileProjection.Unproject(tileCoord);
            var tileCoord2 = TileProjection.Project(extent.Center);
            var extent2 = TileProjection.Unproject(tileCoord2);
            Assert.AreEqual(tileCoord, tileCoord2, "Unproject -> Project したら元に戻る");
            Assert.AreEqual(extent, extent2, "Unproject と Unproject -> Project -> Unproject は同じ");
        }
    }
}
