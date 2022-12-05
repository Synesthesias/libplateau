using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class CityModelPackageTest
    {
        [TestMethod]
        public void TestPackageToJapaneseName()
        {
            var noneFlag = (PredefinedCityModelPackage)0u;
            Assert.AreEqual("なし", noneFlag.ToJapaneseName());
            
            var flags = (PredefinedCityModelPackage)0b10000000000000000000000011111111;
            Assert.AreEqual(
                "建築物, 道路, 都市計画決定情報, 土地利用, 都市設備, 植生, 土地起伏, 災害リスク, その他",
                flags.ToJapaneseName());
        }
        
    }
}
