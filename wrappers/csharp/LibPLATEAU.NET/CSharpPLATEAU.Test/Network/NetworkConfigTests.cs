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
            Assert.AreEqual("https://plateauapimockv3-1-w3921743.deta.app", NetworkConfig.MockServerUrl);
        }
    }
}
