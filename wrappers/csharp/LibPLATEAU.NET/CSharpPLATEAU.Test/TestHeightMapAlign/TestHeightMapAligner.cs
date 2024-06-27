using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
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
        using var aligner = HeightMapAligner.Create();
        aligner.AddHeightmapFrame(emptyMap, 0, 0, 0, 0,0 , 0, 0, 0);
        aligner.Align(model);
    }
}
