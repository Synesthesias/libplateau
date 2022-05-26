
using System;
using System.IO;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public static class CityGml
    {
        public static CityModel Load(string gmlPath, CitygmlParserParams parserParams)
        {
            APIResult result = NativeMethods.plateau_load_citygml(gmlPath, parserParams, out IntPtr cityModelHandle);
            if (result == APIResult.ErrorLoadingCityGml)
            {
                throw new FileLoadException(
                    $"Loading gml failed.\nPlease check codelist xml files are located in (gmlFolder)/../../codelistsgml\nAND gml file is located at {gmlPath}\nand ");
            }
            DLLUtil.CheckDllError(result);
            return new CityModel(cityModelHandle);
        }
    }
}
