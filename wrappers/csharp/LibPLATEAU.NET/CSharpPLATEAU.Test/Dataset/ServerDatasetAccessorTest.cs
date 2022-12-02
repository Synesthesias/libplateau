using System;
using System.Diagnostics;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Interop;
using PLATEAU.Network;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class ServerDatasetAccessorTest
    {
        [TestMethod]
        public void GetMetadataGroup_Returns_Array_Of_MetadataGroup()
        {
            var accessor = ServerDatasetAccessor.Create("");
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
            Assert.AreEqual(3, dataset.MaxLOD);
            accessor.Dispose();
        }

        [TestMethod]
        public void MetadataContainsFeatureTypes()
        {
            var accessor = ServerDatasetAccessor.Create("");
            var meta = accessor.GetMetadataGroup().At(0).Datasets.At(1);
            Assert.AreEqual("八王子市", meta.Title);
            var featureTypes = meta.FeatureTypes.ToCSharpArray();
            CollectionAssert.AreEquivalent(new []{"bldg", "dem"},featureTypes);
        }

        [TestMethod]
        public void GetMeshCodes_Returns_MeshCodes()
        {
            var accessor = ServerDatasetAccessor.Create("23ku");
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(2, meshCodes.Length);
            Assert.AreEqual("53392642", meshCodes.At(0).ToString());
            accessor.Dispose();
        }

        [TestMethod]
        public void GetGmlFiles_Works()
        {
            var accessor = ServerDatasetAccessor.Create("23ku");
            var gmlFiles = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmlFiles.Length);
            var gml = gmlFiles.At(0);
            Assert.AreEqual(NetworkConfig.MockServerURL + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392670_bldg_6697_2_op.gml", gml.Path);
            accessor.Dispose();
        }

        [TestMethod]
        public void GetGmlFiles_Cache_Works()
        {
            var accessor = ServerDatasetAccessor.Create("23ku");
            var stopwatch = Stopwatch.StartNew();
            var gmlFiles = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmlFiles.Length);
            stopwatch.Stop();
            var time1 = stopwatch.Elapsed;
            Console.WriteLine($"{time1} sec");
            stopwatch.Reset();
            stopwatch.Start();
            gmlFiles = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            stopwatch.Stop();
            var time2 = stopwatch.Elapsed;
            Console.WriteLine($"{time2} sec");
            Assert.AreEqual(1, gmlFiles.Length);
            Assert.IsTrue((time1 - time2).TotalMilliseconds > 500, "キャッシュにより、1回目より2回目のほうが速い（ネットワークアクセスが省略される）");
        }

        [TestMethod]
        public void GetPackages_Works()
        {
            var accessor = ServerDatasetAccessor.Create("23ku");
            var buildings = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Building);
            var roads = accessor.GetGmlFiles(Extent.All, PredefinedCityModelPackage.Road);
            Assert.AreEqual(1, buildings.Length);
            var expected = PredefinedCityModelPackage.Building | PredefinedCityModelPackage.Road | PredefinedCityModelPackage.UrbanPlanningDecision | PredefinedCityModelPackage.LandUse;
            Assert.AreEqual(expected, accessor.Packages);
        }
    }
}
