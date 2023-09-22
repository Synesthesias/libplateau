using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Texture;

namespace PLATEAU.Test.Texture
{
    [TestClass]
    public class MapZoomLevelSearcherTest
    {
        [TestMethod]
        public void SearchValidZoomLevel()
        {
            string urlTemplate = "https://cyberjapandata.gsi.go.jp/xyz/relief/{z}/{x}/{y}.png";
            double latitude = 35.6771964003015;
            double longitude = 139.74784608229484;
            var result = new MapZoomLevelSearcher().Search(urlTemplate, latitude, longitude);
            Assert.AreEqual(5, result.AvailableZoomLevelMin);
            Assert.AreEqual(15, result.AvailableZoomLevelMax);
            Assert.IsTrue(result.IsValid);
        }

        [TestMethod]
        public void SearchInvalidZoomLevel()
        {
            string urlTemplate = "https://invalid_url.com/{z}/{x}/{y}.png";
            double latitude = 35.6771964003015;
            double longitude = 139.74784608229484;
            var result = new MapZoomLevelSearcher().Search(urlTemplate, latitude, longitude);
            Assert.IsFalse(result.IsValid);
        }
    }
}
