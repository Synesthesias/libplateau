using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Network;

namespace PLATEAU.Test.Network
{
    [TestClass]
    public class ClientTest
    {
        [TestMethod]
        public void GetDatasetMetadataGroup()
        {
            var client = Client.Create();
            client.Url = NetworkConfig.DefaultApiServerUrl;
            var metadataGroups = client.GetDatasetMetadataGroup();
            Assert.AreEqual(2, metadataGroups.Length);
            var group = metadataGroups.At(0);
            Assert.AreEqual("tokyo", group.ID);
            Assert.AreEqual("東京都", group.Title);
            var dataset = group.Datasets.At(0);
            Assert.AreEqual("23ku", dataset.ID);
            Assert.AreEqual("23区", dataset.Title);
            Assert.AreEqual("xxxx", dataset.Description);
            Assert.AreEqual(3, dataset.MaxLOD);
            client.Dispose();
        }

        [TestMethod]
        public void GetAndSetUrl()
        {
            var client = Client.Create();
            client.Url = "https://dummy.com";
            Assert.AreEqual("https://dummy.com", client.Url);
            client.Dispose();
        }
    }
}
