using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class AppearanceTests
    {
        private static CityModel cityModel;
        private static Appearance appearance;

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            // 探索して最初に見つかった Appearance をテスト対象にします。
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            appearance = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureThemesCountOfFront(true) > 0)
                .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemeNames(true)[0], true))
                .Select(ttd => ttd.Appearance)
                .First();
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void Type_Returns_Not_Empty_String()
        {
            string type = appearance.Type;
            Console.WriteLine($"type = {type}");
            Assert.IsTrue(type.Length > 0);
        }

        [TestMethod]
        public void IsFront_Returns_Good_Bool()
        {
            bool isFront = appearance.IsFront;
            Assert.AreEqual(true, isFront);
        }

        [TestMethod]
        public void Themes_Returns_More_Than_Zero_Themes()
        {
            foreach (string theme in appearance.Themes)
            {
                Console.WriteLine($"theme: {theme}");
            }

            Assert.IsTrue(appearance.Themes.Length > 0);
        }
    }
}
