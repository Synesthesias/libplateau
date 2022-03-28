using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibPLATEAU.NET
{
    public static class CityGml
    {
        public static CityModel Load(string gmlPath, CitygmlParserParams parserParams)
        {
            return new CityModel(NativeMethods.plateau_load_citygml(gmlPath, parserParams));
        }
    }
}
