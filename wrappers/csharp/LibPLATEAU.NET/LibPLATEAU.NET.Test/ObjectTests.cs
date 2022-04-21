using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Object = LibPLATEAU.NET.CityGML.Object;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjectTests
    {
        private Object plateauObject;

        public ObjectTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            plateauObject = cityModel.RootCityObjects[0];
        }
    }
}