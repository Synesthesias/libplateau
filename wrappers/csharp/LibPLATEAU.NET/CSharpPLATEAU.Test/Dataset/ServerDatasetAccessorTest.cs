using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Interop;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class ServerDatasetAccessorTest
    {
        [TestMethod]
        public void GetMetadataGroup_Returns_Array_Of_MetadataGroup()
        {
            var accessor = ServerDatasetAccessor.Create();
            var metadataGroups = accessor.GetMetadataGroup();
            Assert.AreEqual(2, metadataGroups.Length);
            var metadataGroup = metadataGroups.At(0);
            Assert.AreEqual("tokyo", metadataGroup.ID);
            Assert.AreEqual("東京都", metadataGroup.Title);
            var datasets = metadataGroup.Datasets;
            var dataset = datasets.At(0);
            Assert.AreEqual("23ku", dataset.ID);
            Assert.AreEqual("23区", dataset.Title);
            Assert.AreEqual("xxxx", dataset.Description);
            accessor.Dispose();
        }

        [TestMethod]
        public void GetMeshCodes_Returns_MeshCodes()
        {
            var accessor = ServerDatasetAccessor.Create();
            accessor.SetDatasetID("23ku");
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(2, meshCodes.Length);
            Assert.AreEqual("53392642", meshCodes.At(0).ToString());
            accessor.Dispose();
        }

        [TestMethod]
        public void GetGmlFiles_Works()
        {
            var accessor = ServerDatasetAccessor.Create();
            accessor.SetDatasetID("23ku");
            var gmlFiles = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            accessor.Dispose();
        }
    }
}
