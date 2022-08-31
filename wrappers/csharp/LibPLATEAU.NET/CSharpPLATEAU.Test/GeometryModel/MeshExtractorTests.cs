using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.GeometryModel;
using PLATEAU.Interop;
using PLATEAU.IO;
using PLATEAU.Test.CityGML;

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
    }
}
