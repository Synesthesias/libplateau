using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Native;

namespace PLATEAU.Test.Interop
{
    [TestClass]
    public class NativeVectorGmlFileTest
    {
        [TestMethod]
        public void CreateAndDisposeWorks()
        {
            var gmlFiles = NativeVectorGmlFile.Create();
            gmlFiles.Dispose();
        }
    }
}
