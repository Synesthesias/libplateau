using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geometries;
using PLATEAU.HeightMapAlign;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.TestHeightMapAlign;

[TestClass]
public class TestHeightMapAligner
{
    [TestMethod]
    public void OnZeroHeightmap()
    {
        using var model = Model.Create();
        var emptyMap = Array.Empty<UInt16>();
        using var aligner = HeightMapAligner.Create(0, CoordinateSystem.EUN);
        Assert.AreEqual(0, aligner.HeightMapCount);
        aligner.AddHeightmapFrame(emptyMap, 0, 0, 0, 0,0 , 0, 0, 0, CoordinateSystem.EUN);
        Assert.AreEqual(1, aligner.HeightMapCount);
        aligner.AlignInvert(model);
        aligner.Align(model);
    }
}
