using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass]
public class AppearanceTargetTests
{
    private AppearanceTarget appTargetWithTheme;
    
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
    public void TextureThemes()
    {
        foreach(string theme in this.appTargetWithTheme.TextureThemes(true))
        {
            Console.WriteLine($"TextureTheme: {theme}");
            Assert.AreEqual("rgbTexture", theme);
        }
    }
}