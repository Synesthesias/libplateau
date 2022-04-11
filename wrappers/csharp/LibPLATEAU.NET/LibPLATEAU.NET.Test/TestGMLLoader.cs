namespace LibPLATEAU.NET.Test {
    public static class TestGMLLoader {
        private const string gmlPath = "data/53392642_bldg_6697_op2.gml";

        public static CityModel LoadTestGMLFile() {
            var parserParams = new CitygmlParserParams
            {
                Optimize = 0
            };
            var cityModel = CityGml.Load(gmlPath, parserParams);
            return cityModel;
        }
    }
}