using System;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test;

[TestClass]
public class TextureTargetDefinitionTests
{
    private TextureTargetDefinition texTarget;
    public TextureTargetDefinitionTests()
    {
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
        int actualCount = this.texTarget.TextureCoordinatesCount;
        Console.WriteLine($"TextureCoordinatesCount : {actualCount}");
        Assert.IsTrue(actualCount > 0);
    }
}