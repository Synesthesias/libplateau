using System.Collections.Generic;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Test {
    public static class TestGMLLoader {

        private static readonly Dictionary<GmlFileCase, string> gmlPathes = new()
        {
            { GmlFileCase.Simple, "data/53392642_bldg_6697_op2.gml" },
            // { GmlFileCase.Minatomirai , "data/53392642_bldg_6697_op2.gml"} // 今のところ未使用
        };
        
        public enum GmlFileCase{ Simple, Minatomirai }
        

        /// <summary>
        /// テスト用のGMLファイルをロードして <see cref="CityModel"/> を返します。
        /// </summary>
        public static CityModel LoadTestGMLFile(GmlFileCase gmlFileCase, int optimize = 1, bool tesselate = true)
        {
            var parserParams = new CitygmlParserParams(optimize, tesselate);
            var cityModel = CityGml.Load(gmlPathes[gmlFileCase], parserParams);
            return cityModel;
        }
    }
}