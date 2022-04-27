
namespace LibPLATEAU.NET.CityGML
{
    public static class CityGml
    {
        public static CityModel Load(string gmlPath, CitygmlParserParams parserParams)
        {
            return new CityModel(NativeMethods.plateau_load_citygml(gmlPath, parserParams));
        }
    }
}
