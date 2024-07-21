using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.GranularityConvert;
using PLATEAU.PolygonMesh;
using PLATEAU.Test.GeometryModel;

namespace PLATEAU.Test.GranularityConvert
{
    [TestClass]
    public class GranularityConverterTest
    {
        [TestMethod]
        public void ConvertSucceeds()
        {
            var srcModel = TestGeometryUtil.ExtractModel();
            var converter = new GranularityConverter();
            var option = new GranularityConvertOption(ConvertGranularity.PerCityModelArea, 10);
            var dstModel = converter.Convert(srcModel, option);
        }

    }
}
