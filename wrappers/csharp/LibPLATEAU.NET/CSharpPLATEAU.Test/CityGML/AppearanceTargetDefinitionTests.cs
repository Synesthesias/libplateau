using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class AppearanceTargetDefinitionTests
    {
        private static CityModel cityModel;
        private static TextureTargetDefinition texTargetDef;

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            // 探索して最初に見つかった TextureTargetDefinition をテスト対象にします。
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            texTargetDef = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureThemesCountOfFront(true) > 0)
                .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemeNames(true)[0], true))
                .First();
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void TargetID_Returns_Not_Empty()
        {
            string targetID = texTargetDef.TargetID;
            Console.WriteLine($"targetID = {targetID}");
            Assert.IsTrue(targetID.Length > 0);
        }

        [TestMethod]
        public void Appearance_Returns_NotNull()
        {
            var appearance = texTargetDef.Appearance;
            Assert.IsNotNull(appearance);
        }
    }
}
