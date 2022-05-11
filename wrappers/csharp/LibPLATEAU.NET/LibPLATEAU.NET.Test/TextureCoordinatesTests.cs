using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass]
public class TextureCoordinatesTests
{
    private TextureCoordinates texCoords;
    
    // 前準備
    public TextureCoordinatesTests()
    {
        // 探索して最初に見つかった TextureCoordinates をテスト対象にします。
        var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
        this.texCoords = cityModel.RootCityObjects
            .SelectMany(co => co.CityObjectDescendantsDFS)
            .SelectMany(co => co.Geometries)
            .SelectMany(geom => geom.GeometryDescendantsDFS)
            .SelectMany(geom => geom.Polygons)
            .Where(poly => poly.TextureThemesCount(true) > 0)
            .Select(poly => poly.GetTextureTargetDefinition(poly.TextureThemes(true)[0], true))
            .Where(texTarget => texTarget.TexCoordinatesCount > 0)
            .Select(texTarget => texTarget.GetCoordinate(0))
            .First();
    }

    [TestMethod]
    public void Vec2CoordsCount_Returns_Positive_Number()
    {
        int count = this.texCoords.Vec2CoordsCount;
        Console.WriteLine($"Vec2CoordsCount = {count}");
        Assert.IsTrue(count > 0);
    }

    [TestMethod]
    public void GetVec2Coord_Is_Not_Zero()
    {
        var vec2 = this.texCoords.GetVec2Coord(0);
        Console.WriteLine($"GetVec2Coord(0) = {vec2}");
        Assert.IsTrue(vec2.IsNotZero());
    }
    
    
}