using System;
using System.Collections.Generic;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Test {
    public static class TestUtil {

        private static readonly Dictionary<GmlFileCase, string> GMLPathes = new()
        {
            { GmlFileCase.Simple, "data/53392642_bldg_6697_op2.gml" },
            // { GmlFileCase.Minatomirai , "data/53392642_bldg_6697_op2.gml"} // 今のところ未使用
        };
        
        public enum GmlFileCase{ Simple, Minatomirai }
        
        /// <summary>
        /// テスト用のGMLファイルをロードして <see cref="CityModel"/> を返します。
        /// </summary>
        public static CityModel LoadTestGMLFile(GmlFileCase gmlFileCase, int optimize = 1, bool tessellate = true)
        {
            var parserParams = new CitygmlParserParams(optimize, tessellate);
            var cityModel = CityGml.Load(GMLPathes[gmlFileCase], parserParams);
            return cityModel;
        }

        /// <summary>
        /// <see cref="PlateauVector3d"/> の拡張メソッドです。
        /// X , Y , Z のうちいずれかが 0 に近い値であれば false を返します。
        /// そうでなければ true を返します。
        /// </summary>
        public static bool IsNotZero(this PlateauVector3d pos)
        {
            bool isNotZero = true;
            isNotZero &= Math.Abs(pos.X) > 0.01;
            isNotZero &= Math.Abs(pos.Y) > 0.01;
            isNotZero &= Math.Abs(pos.Z) > 0.01;
            return isNotZero;
        }
    }
}