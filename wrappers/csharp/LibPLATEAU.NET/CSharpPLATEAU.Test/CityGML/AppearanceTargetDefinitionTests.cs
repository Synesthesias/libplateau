using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class AppearanceTargetDefinitionTests
    {
        private readonly TextureTargetDefinition texTargetDef;

        // 前準備
        public AppearanceTargetDefinitionTests()
        {
            // 探索して最初に見つかった TextureTargetDefinition をテスト対象にします。
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            this.texTargetDef = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureThemesCountOfFront(true) > 0)
                .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemeNames(true)[0], true))
                .First();
        }

        [TestMethod]
        public void TargetID_Returns_Not_Empty()
        {
            string targetID = this.texTargetDef.TargetID;
            Console.WriteLine($"targetID = {targetID}");
            Assert.IsTrue(targetID.Length > 0);
        }

        [TestMethod]
        public void Appearance_Returns_NotNull()
        {
            var appearance = this.texTargetDef.Appearance;
            Assert.IsNotNull(appearance);
        }
    }
}