using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.Dataset;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;
using PLATEAU.Test.CityGML;
using System.Collections.Generic;

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

        [TestMethod]
        public void ExtractInExtentsReturnsAllNodesIfItIsCalledWithSufficientExtents()
        {
            var options = MeshExtractOptions.DefaultValue();

            using var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            options.SetLODRange(0, 2);
            options.EnableTexturePacking = true;

            var allModel = Model.Create();
            MeshExtractor.Extract(ref allModel, cityModel, options);

            var thirdMeshCodeStr = "53392642";
            List<Extent> extents = new List<Extent>();
            for (int i = 1; i <= 4; ++i)
            {
                for (int j = 1; j <= 4; ++j)
                {
                    var meshCodeStr = thirdMeshCodeStr + $"{i}{j}";
                    extents.Add(MeshCode.Parse(meshCodeStr).Extent);
                }
            }

            var model = Model.Create();
            MeshExtractor.ExtractInExtents(ref model, cityModel, options, extents);

            for (int i = 0; i <= 2; ++i)
            {
                Assert.IsTrue(allModel.GetRootNodeAt(i).ChildCount > 0, "少なくとも１つのノードが出力されている");
                Assert.AreEqual(allModel.GetRootNodeAt(i).ChildCount, model.GetRootNodeAt(i).ChildCount);
            }
        }

        //    const auto model = MeshExtractor::extractInExtents(*city_model_, options, extents);
        //    std::vector actual_node_counts = { 0, 0, 0 };

        //    for (int i = 0; i <= 2; ++i)
        //    {
        //        ASSERT_GT(all_model->getRootNodeAt(i).getChildCount(), 0);
        //        ASSERT_EQ(all_model->getRootNodeAt(i).getChildCount(), model->getRootNodeAt(i).getChildCount());
        //    }
        //}

    }
}
