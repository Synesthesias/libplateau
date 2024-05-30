using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.HeightMapAlign;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.TestHeightMapAlign;

[TestClass]
public class TestHeightMapAligner
{
    [TestMethod]
    public void OnEmpty()
    {
        var model = Model.Create();
        var emptyMap = Array.Empty<UInt16>();
        new HeightMapAligner().Align(
            model, emptyMap, 0, 0, 0, 0,0 , 0, 0, 0
            );
        model.Dispose();
    }
}
