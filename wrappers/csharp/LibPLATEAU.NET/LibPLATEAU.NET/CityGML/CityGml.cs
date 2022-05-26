
using System;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public static class CityGml
    {
        public static CityModel Load(string gmlPath, CitygmlParserParams parserParams)
        {
            APIResult result = NativeMethods.plateau_load_citygml(gmlPath, parserParams, out IntPtr cityModelHandle);
            DLLUtil.CheckDllError(result);
            return new CityModel(cityModelHandle);
        }
    }
}
