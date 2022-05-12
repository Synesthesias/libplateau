using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass]
public class AppearanceTests
{
    private Appearance appearance;
    
    public AppearanceTests()
    {
        // 探索して最初に見つかった Appearance をテスト対象にします。
        var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        this.appearance = cityModel.RootCityObjects
            .SelectMany(co => co.CityObjectDescendantsDFS)
            .SelectMany(co => co.Geometries)
            .SelectMany(geom => geom.GeometryDescendantsDFS)
            .SelectMany(geom => geom.Polygons)
            .Where(poly => poly.TextureThemesCount(true) > 0)
            .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemes(true)[0], true))
            .Select(ttd => ttd.Appearance)
            .First();
    }

    [TestMethod]
    public void Type_Returns_Not_Empty_String()
    {
        string type = this.appearance.Type;
        Console.WriteLine($"type = {type}");
        Assert.IsTrue(type.Length > 0);
    }

    [TestMethod]
    public void IsFront_Returns_Good_Bool()
    {
        bool isFront = this.appearance.IsFront;
        Assert.AreEqual(true, isFront);
    }
}