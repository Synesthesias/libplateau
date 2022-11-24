using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Interop;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class DatasetAccessorTest
    {
        [TestMethod]
        public void TestGetGmlFiles()
        {
            using var datasetSource = DatasetSource.CreateLocal("data");
            var accessor = datasetSource.Accessor;
            var gmls = accessor.GetGmlFiles(
                new Extent(
                    new GeoCoordinate(35.53, 139.77, -9999),
                    new GeoCoordinate(35.54, 139.78, 9999)
                ),
                PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmls.Length);
        }

        [TestMethod]
        public void TestMeshCodesGetter()
        {
            using var datasetSource = DatasetSource.CreateLocal("data");
            var accessor = datasetSource.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
        }
        
        [TestMethod]
        public void TestGetMaxLod()
        {
            using var datasetSource = DatasetSource.CreateLocal("data");
            var accessor = datasetSource.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
            int maxLod = accessor.GetMaxLod(meshCodes.At(0), PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, maxLod);
        }
        
    }
}
