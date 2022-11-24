using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class ServerDatasetAccessorTest
    {
        [TestMethod]
        public void GetMetadataGroup_Returns_Array_Of_MetadataGroup()
        {
            var accessor = ServerDatasetAccessor.Create();
            var metadataGroup = accessor.GetMetadataGroup();
            Assert.AreEqual(2, metadataGroup.Length);
            var metadata = metadataGroup.At(0);
            Assert.AreEqual("tokyo", metadata.ID);
            Assert.AreEqual("東京都", metadata.Title);
            accessor.Dispose();
        }
    }
}
