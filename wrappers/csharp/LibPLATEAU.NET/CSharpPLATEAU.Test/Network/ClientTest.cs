using Microsoft.VisualStudio.TestTools.UnitTesting;
using NuGet.Frameworks;
using PLATEAU.Network;

namespace PLATEAU.Test.Network
{
    [TestClass]
    public class ClientTest
    {
        private const string MockServerUrl = "https://9tkm2n.deta.dev";
        [TestMethod]
        public void DatasetMetadataGroup()
        {
            using var client = Client.Create();
            client.Url = MockServerUrl;
            var metadataGroup = client.GetDatasetMetadataGroup();
            Assert.AreEqual("東京都", metadataGroup.At(0).Title);
            Assert.AreEqual("23区", metadataGroup.At(0).Datasets.At(0).Title);
        }

        [TestMethod]
        public void GetAndSetUrl()
        {
            using var client = Client.Create();
            client.Url = "https://dummy.com";
            Assert.AreEqual("https://dummy.com", client.Url);
        }
    }
}
