using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Network;

namespace PLATEAU.Test.Network
{
    [TestClass]
    public class NetworkConfigTests
    {

        [TestMethod]
        public void GetDefaultApiServerUrl()
        {
            Assert.AreEqual("https://api.plateau.reearth.io", NetworkConfig.DefaultApiServerUrl);
        }

        [TestMethod]
        public void GetDefaultApiToken()
        {
            Assert.AreEqual("secret-56c66bcac0ab4724b86fc48309fe517a", NetworkConfig.DefaultApiToken);
        }

        [TestMethod]
        public void GetMockServerUrl()
        {
            Assert.AreEqual("https://plateau-api-mock-v2.deta.dev", NetworkConfig.MockServerUrl);
        }
    }
}
