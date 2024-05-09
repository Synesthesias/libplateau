using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Texture;

namespace PLATEAU.Test.Texture
{
    [TestClass]
    public class HeightmapGeneratorTest
    {
        [TestMethod]
        public void ConvertTo2DFloatArrayTest()
        {
            ushort[] data = new ushort[4] { 65535, 0, 0, 65535 };
            float[,] outData = HeightmapGenerator.ConvertTo2DFloatArray(data, 2, 2);
            Assert.AreEqual(0f, outData[0, 0]);
            Assert.AreEqual(1f, outData[0, 1]);
            Assert.AreEqual(1f, outData[1, 0]);
            Assert.AreEqual(0f, outData[1, 1]);
        }
        
    }
}
