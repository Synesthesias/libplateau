using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class CityObjectTests
    {
        private const string gmlPath = "data/53392642_bldg_6697_op2.gml";

        private readonly CityModel cityModel;
        private readonly CityObject cityObject;

        public CityObjectTests()
        {
            // Load処理が重いため最初のみ実行して使いまわす。
            var parserParams = new CitygmlParserParams
            {
                Optimize = 0
            };
            cityModel = CityGml.Load(gmlPath, parserParams);
            cityObject = cityModel.RootCityObjects[0];
        }

        [TestMethod]
        public void ID_Returns_GML_ID()
        {
            Assert.AreEqual(
                "BLD_0772bfd9-fa36-4747-ad0f-1e57f883f745",
                this.cityObject.ID);
        }

        [TestMethod]
        public void Type_Returns_GML_Type()
        {
            Assert.AreEqual(
                CityObjectType.COT_Building,
                this.cityObject.Type);
        }
    }
}