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

        [TestMethod]
        public void GetAndSetGameMaterialID()
        {
            var subMesh = SubMesh.Create(0, 2, "test");
            Assert.AreEqual(-1, subMesh.GameMaterialID, "GameMaterialIDの初期値は-1です");
            subMesh.GameMaterialID = 1;
            Assert.AreEqual(1, subMesh.GameMaterialID, "GameMaterialIDのgetterとsetterが動作します");
        }
    }
}
