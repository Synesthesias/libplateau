using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass, Ignore]
public class TextureTargetDefinitionTests
{
    private TextureTargetDefinition texTarget;
    
    // 前準備
    public TextureTargetDefinitionTests()
    {
        // 探索して最初に見つかった TextureTargetDefinition をテスト対象にします。
        var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        this.texTarget = cityModel.RootCityObjects
            .SelectMany(co => co.CityObjectDescendantsDFS)
            .SelectMany(co => co.Geometries)
            .SelectMany(geom => geom.GeometryDescendantsDFS)
            .SelectMany(geom => geom.Polygons)
            .Where(poly => poly.TextureThemesCount(true) > 0)
            .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemes(true)[0], true))
            .First();
    }

    [TestMethod]
    public void TextureCoordinatesCount_Returns_Positive_Number()
    {
        int actualCount = this.texTarget.TexCoordinatesCount;
        Console.WriteLine($"TextureCoordinatesCount : {actualCount}");
        Assert.IsTrue(actualCount > 0);
    }

    [TestMethod]
    public void GetTextureCoordinates_Returns_NotNull()
    {
        var coords = this.texTarget.GetCoordinate(0);
        Assert.IsNotNull(coords);
    }
}