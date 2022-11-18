using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class DatasetAccessorTest
    {
        [TestMethod]
        public void TestGetGmlFiles()
        {
            var localAccessor = LocalDatasetAccessor.Find("data");
            // TODO localAccessor を直接渡せるようにする
            var accessor = DatasetAccessor.Create(localAccessor.Handle);
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
            var localAccessor = LocalDatasetAccessor.Find("data");
            var accessor = DatasetAccessor.Create(localAccessor.Handle);
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
        }

        [TestMethod]
        public void TestGetMaxLod()
        {
            var localAccessor = LocalDatasetAccessor.Find("data");
            var accessor = DatasetAccessor.Create(localAccessor.Handle);
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
            int maxLod = accessor.MaxLod(meshCodes[0], PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, maxLod);
        }
        
    }
}
