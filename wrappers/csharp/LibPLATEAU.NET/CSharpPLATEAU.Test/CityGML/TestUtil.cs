using System;
using System.Collections.Generic;
using PLATEAU.CityGML;
using PLATEAU.Interop;

namespace PLATEAU.Test.CityGML {
    public static class TestUtil {

        private static readonly Dictionary<GmlFileCase, string> GmlPaths = new Dictionary<GmlFileCase, string>()
        {
            { GmlFileCase.Simple, "data/udx/bldg/53392642_bldg_6697_op2.gml" },
            // { GmlFileCase.Minatomirai , "data/53392642_bldg_6697_op2.gml"} // 今のところ未使用
        };
        
        public enum GmlFileCase{ Simple, Minatomirai }

        public static string GetGmlPath(GmlFileCase gmlFileCase)
            => GmlPaths[gmlFileCase];

        /// <summary>
        /// テスト用のGMLファイルをロードして <see cref="CityModel"/> を返します。
        /// </summary>
        public static CityModel LoadTestGMLFile(GmlFileCase gmlFileCase, bool optimize = true, bool tessellate = true, bool ignoreGeometries = false)
        {
            var parserParams = new CitygmlParserParams(optimize, tessellate, ignoreGeometries);
            var cityModel = CityGml.Load(GmlPaths[gmlFileCase], parserParams, LogCallbacks.StdOut);
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

        /// <summary>
        /// <see cref="PlateauVector2f"/> の拡張メソッドです。
        /// X, Y のうちいずれかが 0 に近い値であれば false を返します。
        /// そうでなければ true を返します。
        /// </summary>
        public static bool IsNotZero(this PlateauVector2f pos)
        {
            bool isNotZero = true;
            isNotZero &= Math.Abs(pos.X) > 0.01;
            isNotZero &= Math.Abs(pos.Y) > 0.01;
            return isNotZero;
        }
    }
}
