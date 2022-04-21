using System;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Debug
{
    /// <summary>
    /// デバッグ用です。
    /// ユニットテストとは別にして詳しく動作確認したい処理を記します。
    /// </summary>
    public class DebugRun
    {
        private const string GmlPath = "data/53392642_bldg_6697_op2.gml";
        static void Main()
        {
            var parserParams = new CitygmlParserParams()
            {
                Optimize = 0
            };
            var cityModel = CityGml.Load(GmlPath, parserParams);
            var cityObject = cityModel.RootCityObjects[0];
            var attrDict = cityObject.AttributesDictionary;
            Console.WriteLine(string.Join(", ", attrDict.Keys));
        }
    }
}