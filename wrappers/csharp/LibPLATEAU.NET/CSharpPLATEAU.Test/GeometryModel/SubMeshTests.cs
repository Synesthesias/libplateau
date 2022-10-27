using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.GeometryModel
{
    [TestClass]
    public class SubMeshTests
    {
        [TestMethod]
        public void DisposeThenInvalid()
        {
            var subMesh = SubMesh.Create(0, 2, "test");
            Assert.AreEqual("test", subMesh.TexturePath);
            subMesh.Dispose();
            Assert.ThrowsException<Exception>(() => subMesh.TexturePath, "Dispose後は情報にアクセスできない");
        }
    }
}
