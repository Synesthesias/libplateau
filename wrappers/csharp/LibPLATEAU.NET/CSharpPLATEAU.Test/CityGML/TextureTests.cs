using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class TextureTests
    {
        private Texture texture;

        // 前準備
        public TextureTests()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            this.texture = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureTargetDefinitionsCount > 0)
                .Select(poly => poly.GetTextureTargetDefinition(0))
                .Select(ttd => ttd.Appearance)
                .First();
        }

        [TestMethod]
        public void Url_Returns_Not_Empty()
        {
            string url = this.texture.Url;
            Console.WriteLine($"Texture URL: {url}");
            Assert.IsTrue(url.Length > 0);
        }

        [TestMethod]
        public void WrapMode()
        {
            TextureWrapMode wrapMode = this.texture.WrapMode;
            Console.WriteLine(wrapMode);
            Assert.AreEqual(TextureWrapMode.WM_None, wrapMode);
        }
    }
}