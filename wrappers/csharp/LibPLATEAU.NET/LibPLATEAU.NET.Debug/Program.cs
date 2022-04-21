﻿using System;
using System.Net.Mime;
using System.Reflection;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Debug
{
    /// <summary>
    /// デバッグ用です。
    /// ユニットテストとは別にして詳しく動作確認したい処理を記します。
    /// </summary>
    public class DebugRun
    {
        static void Main()
        {
            string gmlPath = Assembly.GetExecutingAssembly().Location + "/../data/53392642_bldg_6697_op2.gml";
            var parserParams = new CitygmlParserParams()
            {
                Optimize = 0
            };
            var cityModel = CityGml.Load(gmlPath, parserParams);
            var cityObject = cityModel.RootCityObjects[0];
            var attrDict = cityObject.AttributesDictionary;
            Console.WriteLine(string.Join(", ", attrDict.Keys));
        }
    }
}