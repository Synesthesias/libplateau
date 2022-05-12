using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

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
            .Where(poly => poly.TextureThemesCount(true) > 0)
            .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemes(true)[0], true))
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
        Console.WriteLine($"Appearance: Texture URL: {appearance.Url}");
        Assert.IsNotNull(appearance);
    }
}