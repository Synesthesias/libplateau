using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Basemap;
using PLATEAU.Interop;

namespace PLATEAU.Test.Basemap
{
    [TestClass]
    public class VectorTileDownloaderTest
    {
        private static readonly string TestDestDir = Path.GetFullPath("./tempTestDestDir");

        [TestInitialize]
        public void TestInitialize()
        {
            Directory.CreateDirectory(TestDestDir);
        }

        [TestCleanup]
        public void TestCleanup()
        {
            
            Directory.Delete(TestDestDir, true);
        }
        
        [TestMethod]
        public void Download_Then_Png_Files_Exist()
        {
            // 東京ビッグサイト周辺
            var extent = new Extent(
                new GeoCoordinate(35.6297, 139.78, -9999),
                new GeoCoordinate(35.6345, 139.80, 9999)
            );
            using var downloader = VectorTileDownloader.Create(
                TestDestDir, extent);
            int tileCount = downloader.TileCount;
            Assert.AreEqual(4, tileCount);
            for (int i = 0; i < tileCount; i++)
            {
                downloader.Download(i, out var _, out string path);
                Assert.AreEqual(path, downloader.CalcDestPath(i));
            }

            string[] filesShouldExist =
            {
                "15/29107/12908.png",
                "15/29107/12909.png",
                "15/29108/12908.png",
                "15/29108/12909.png"
            };
            foreach(var relativePath in filesShouldExist)
            {
                var path = Path.Combine(TestDestDir, relativePath);
                Assert.IsTrue(File.Exists(path));
                var fileInfo = new FileInfo(path);
                Assert.IsTrue(fileInfo.Length > 0);
            }
        }
    }
}
