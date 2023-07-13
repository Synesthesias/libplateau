using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.GeometryModel
{
    [TestClass]
    public class MeshExtractorTests
    {

        [TestMethod]
        public void Extract_Returns_Model_With_Nodes_With_Mesh()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            Assert.IsNotNull(foundMesh, "頂点と面が1つ以上あるメッシュが存在する");
        }

        [TestMethod]
        public void Mesh_Is_Null_When_Node_Has_No_Mesh()
        {
            var model = TestGeometryUtil.ExtractModel();
            var rootNode = model.GetRootNodeAt(0);
            Assert.IsNull(rootNode.Mesh, "メッシュがない状況ではMeshはnullを返す");
        }


        [TestMethod]
        public void CityObjectIndexExistsForEveryUV4Value()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            var cityObjectList = foundMesh.CityObjectList;
            foreach (var vt in foundMesh.GetUv4())
            {
                var atomicGmlID = cityObjectList.GetAtomicID(CityObjectIndex.FromUV(vt));
                var primaryGmlID = cityObjectList.GetPrimaryID(CityObjectIndex.FromUV(vt).PrimaryIndex);
                Assert.IsNotNull(atomicGmlID, "UV4に含まれている値はCityObjectListにも含まれている");
                Assert.IsNotNull(primaryGmlID, "UV4に含まれている値はCityObjectListにも含まれている");
            }
        }

        [TestMethod]
        public void CityObjectIndexCanGetFromGmlID()
        {
            var model = TestGeometryUtil.ExtractModel();
            var foundMesh = TestGeometryUtil.FirstMeshInModel(model);
            var cityObjectList = foundMesh.CityObjectList;
            foreach (var index in cityObjectList.GetAllKeys())
            {
                var gmlID = cityObjectList.GetAtomicID(index);
                var actualIndex = cityObjectList.GetCityObjectIndex(gmlID);

                Assert.AreEqual(index.AtomicIndex, actualIndex.AtomicIndex);
                Assert.AreEqual(index.PrimaryIndex, actualIndex.PrimaryIndex);
            }
        }
    }
}
