using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class AppearanceTargetTests
    {
        private readonly AppearanceTarget appTargetWithTexTheme;
        // private readonly AppearanceTarget appTargetWithMatTheme;

        // 初期化
        public AppearanceTargetTests()
        {
            // テスト対象として適切なものを検索します。
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            this.appTargetWithTexTheme = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .First(poly => poly.TextureThemesCountOfFront(true) > 0);

            //　MaterialTargetDefinition を含むGMLがあれば、下のコメントアウトを外してテストします。
            // 今はGML例が見当たらないのでテスト中止しています。

            // this.appTargetWithMatTheme = cityModel.RootCityObjects
            //     .SelectMany(co => co.CityObjectDescendantsDFS)
            //     .SelectMany(co => co.Geometries)
            //     .SelectMany(geom => geom.GeometryDescendantsDFS)
            //     .First(geom => geom.MaterialThemesCount(true) > 0);
            // .SelectMany(geom => geom.Polygons)
            // .First(poly => poly.MaterialThemesCount(true) > 0);
        }

        [TestMethod]
        public void Do_Exist_AppearanceTarget_That_TextureThemesCount_Is_Positive_Number()
        {
            Console.Write($"TextureThemesCountOfFront: {this.appTargetWithTexTheme.TextureThemesCountOfFront(true)}");
            Assert.IsNotNull(this.appTargetWithTexTheme);
        }

        [TestMethod]
        public void TextureThemes_Returns_GML_ThemeName()
        {
            foreach (string theme in this.appTargetWithTexTheme.TextureThemeNames(true))
            {
                Console.WriteLine($"TextureTheme: {theme}");
                Assert.AreEqual("rgbTexture", theme);
            }
        }

        [TestMethod]
        public void GetTextureTargetDefinition_Returns_Not_Null()
        {
            var ttd = this.appTargetWithTexTheme.GetTextureTargetDefinition("rgbTexture", true);
            Assert.IsNotNull(ttd);
        }

        [TestMethod]
        public void GetTextureTargetDefinition_Throws_Error_When_Not_Found()
        {
            Assert.ThrowsException<KeyNotFoundException>(() =>
                this.appTargetWithTexTheme.GetTextureTargetDefinition("DummyNotFound", true)
            );
        }

        [TestMethod]
        public void TextureThemesCount_Returns_Positive_Number()
        {
            int count = this.appTargetWithTexTheme.TextureTargetDefinitionsCount;
            Console.WriteLine($"TextureThemesCount : {count}");
            Assert.IsTrue(count > 0);
        }

        [TestMethod]
        public void GetTextureTargetDefinition_By_Index_Returns_NotNull()
        {
            var texTargetDef = this.appTargetWithTexTheme.GetTextureTargetDefinition(0);
            Assert.IsNotNull(texTargetDef);
        }

        [TestMethod]
        public void TextureTargetDefinitions_Enumerates_Same_Count()
        {
            int enumerateCount = this.appTargetWithTexTheme.TextureTargetDefinitions.Count();
            int expectedCount = this.appTargetWithTexTheme.TextureTargetDefinitionsCount;
            Assert.AreEqual(expectedCount, enumerateCount);
        }

        //　MaterialTargetDefinition を含むGMLがあれば、下のコメントアウトを外してテストします。
        // 今はGML例が見当たらないのでテスト中止しています。

        // [TestMethod]
        // public void GetMaterialTargetDefinition_Returns_Not_Null()
        // {
        //     Console.WriteLine(this.appTargetWithMatTheme.MaterialThemes(true)[0]);
        //     var mtd = this.appTargetWithMatTheme.GetMaterialTargetDefinition("rgbTexture", true);
        //     Assert.IsNotNull(mtd);
        // }
        //
        // [TestMethod]
        // public void GetMaterialTargetDefinition_Throws_Error_When_Not_Found()
        // {
        //     Assert.ThrowsException<KeyNotFoundException>(() =>
        //         this.appTargetWithMatTheme.GetMaterialTargetDefinition("DummyNotFound", true)
        //     );
        // }
    }
}
