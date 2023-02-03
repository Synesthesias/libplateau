using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Network;

namespace PLATEAU.Test.Network
{
    [TestClass]
    public class NetworkConfigTests
    {

        [TestMethod]
        public void GetMockServerUrl()
        {
            Assert.AreEqual("https://plateau-api-mock-v2.deta.dev", NetworkConfig.MockServerUrl);
        }
    }
}
