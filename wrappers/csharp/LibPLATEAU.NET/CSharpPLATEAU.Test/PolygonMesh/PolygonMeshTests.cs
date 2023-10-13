using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.PolygonMesh
{
    [TestClass]
    public class PolygonMeshTests
    {
        [TestMethod]
        public void Create_And_Add_And_Get()
        {
            using var cityObjList = CityObjectList.Create();
            cityObjList.Add(new CityObjectIndex(0,0), "test-gml-id");
            bool found = cityObjList.TryGetAtomicID(new CityObjectIndex(0, 0), out var gmlID);
            Assert.AreEqual("test-gml-id", gmlID);
            Assert.IsTrue(true);
        }

        [TestMethod]
        public void TryGetAtomicID_Returns_False_If_Not_Found()
        {
            using var cityObjList = CityObjectList.Create();
            bool found = cityObjList.TryGetAtomicID(new CityObjectIndex(0, 0), out var _);
            Assert.IsFalse(found);
        }
    }
}
