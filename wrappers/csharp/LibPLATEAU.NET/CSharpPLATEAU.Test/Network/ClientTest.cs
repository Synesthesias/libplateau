using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Native;
using PLATEAU.Network;

namespace PLATEAU.Test.Network
{
    [TestClass]
    public class ClientTest
    {
        [Ignore, TestMethod]
        public void GetDatasetMetadataGroup()
        {
            var client = Client.CreateForMockServer();
            var metadataGroups = client.GetDatasetMetadataGroup();
            Assert.AreEqual(2, metadataGroups.Length);
            var group = metadataGroups.At(0);
            Assert.AreEqual("tokyo", group.ID);
            Assert.AreEqual("東京都", group.Title);
            var dataset23Ku = group.Datasets.At(0);
            Assert.AreEqual("23ku", dataset23Ku.ID);
            Assert.AreEqual("23区", dataset23Ku.Title);
            Assert.AreEqual("xxxx", dataset23Ku.Description);
            var datasetHachioji = group.Datasets.At(1);
            Assert.AreEqual("八王子市", datasetHachioji.Title);
            var featureTypes = datasetHachioji.FeatureTypes;
            CollectionAssert.AreEquivalent(new[] { "bldg", "dem" }, featureTypes);
            client.Dispose();
            
        }

        [TestMethod]
        public void GetAndSetUrl()
        {
            var client = Client.Create("", "");
            client.Url = "https://dummy.com";
            Assert.AreEqual("https://dummy.com", client.Url);
            client.Dispose();
        }

        [TestMethod]
        public void NativeVectorDatasetMetadataCreateAndDispose()
        {
            var vectorMetadata = NativeVectorDatasetMetadata.Create();
            vectorMetadata.Dispose();
        }
    }
}
