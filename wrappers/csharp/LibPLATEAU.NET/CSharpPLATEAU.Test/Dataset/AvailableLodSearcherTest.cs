using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Editor.CityImport.AreaSelector;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class LodSearcherTest
    {
        [TestMethod]
        public void SearchLodInFile_Returns_Gml_Lods()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var lods = LodSearcher.SearchLodsInFile(path).ToArray();
            CollectionAssert.AreEquivalent(new uint[]{0, 1, 2}, lods);
        }
        
    }
}
