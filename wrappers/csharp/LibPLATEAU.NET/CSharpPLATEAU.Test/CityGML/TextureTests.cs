using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class TextureTests
    {
        private static CityModel cityModel;
        private static Texture texture;

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            texture = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureTargetDefinitionsCount > 0)
                .Select(poly => poly.GetTextureTargetDefinition(0))
                .Select(ttd => ttd.Appearance)
                .First();
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void Url_Returns_Not_Empty()
        {
            string url = texture.Url;
            Console.WriteLine($"Texture URL: {url}");
            Assert.IsTrue(url.Length > 0);
        }

        [TestMethod]
        public void WrapMode()
        {
            TextureWrapMode wrapMode = texture.WrapMode;
            Console.WriteLine(wrapMode);
            Assert.AreEqual(TextureWrapMode.WM_None, wrapMode);
        }
    }
}
