using System;
using System.Collections.Generic;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass]
public class AppearanceTargetTests
{
    private readonly AppearanceTarget appTargetWithTheme;
    
    // 初期化
    public AppearanceTargetTests()
    {
        var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        this.appTargetWithTheme = cityModel.RootCityObjects
            .SelectMany(co => co.CityObjectDescendantsDFS)
            .SelectMany(co => co.Geometries)
            .SelectMany(geom => geom.GeometryDescendantsDFS)
            .SelectMany(geom => geom.Polygons)
            .First(poly => poly.TextureThemesCount(true) > 0);
    }

    [TestMethod]
    public void Do_Exist_AppearanceTarget_That_TextureThemesCount_Is_Positive_Number()
    {
        Console.Write($"TextureThemesCount: {this.appTargetWithTheme.TextureThemesCount(true)}");
        Assert.IsNotNull(this.appTargetWithTheme);
    }

    [TestMethod]
    public void TextureThemes_Returns_GML_ThemeName()
    {
        foreach(string theme in this.appTargetWithTheme.TextureThemes(true))
        {
            Console.WriteLine($"TextureTheme: {theme}");
            Assert.AreEqual("rgbTexture", theme);
        }
    }

    [TestMethod, Ignore]
    public void GetTextureTargetDefinition_Returns_Not_Null()
    {
        var ttd = this.appTargetWithTheme.GetTextureTargetDefinition("rgbTexture", true);
        Assert.IsNotNull(ttd);
    }

    [TestMethod, Ignore]
    public void GetTextureTargetDefinition_Throws_Error_When_Not_Found()
    {
        Assert.ThrowsException<KeyNotFoundException>(() =>
            this.appTargetWithTheme.GetTextureTargetDefinition("DummyNotFound", true)
            );
    }
}