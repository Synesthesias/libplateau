using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Test {
    public static class TestGMLLoader {
        private const string GmlPath = "data/53392642_bldg_6697_op2.gml";

        /// <summary>
        /// テスト用のGMLファイルをロードして <see cref="CityModel"/> を返します。
        /// </summary>
        public static CityModel LoadTestGMLFile() {
            var parserParams = new CitygmlParserParams
            {
                Optimize = 0
            };
            var cityModel = CityGml.Load(GmlPath, parserParams);
            return cityModel;
        }
    }
}