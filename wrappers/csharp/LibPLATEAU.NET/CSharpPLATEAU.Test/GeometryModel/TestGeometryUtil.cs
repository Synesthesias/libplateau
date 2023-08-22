using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.PolygonMesh;
using PLATEAU.Test.CityGML;

namespace PLATEAU.Test.GeometryModel
{
    public static class TestGeometryUtil
    {
        public static Model ExtractModel()
        {
            using var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var options = MeshExtractOptions.DefaultValue();
            options.MinLOD = 2;
            options.MaxLOD = 2;
            options.EnableTexturePacking = true;

            var model = Model.Create();
            MeshExtractor.Extract(ref model, cityModel, options);
            return model;
        }


        public static Mesh FirstMeshInModel(Model model)
        {
            Assert.IsTrue(model.RootNodesCount > 0, "モデル内にルートノードが存在する");
            var rootNode = model.GetRootNodeAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(rootNode.Name), "ルートノードの1つを取得でき、その名前を取得できる");
            Assert.IsTrue(rootNode.ChildCount > 0, "ルートノードの子を取得できる");
            var firstChild = rootNode.GetChildAt(0);
            Assert.IsFalse(string.IsNullOrEmpty(firstChild.Name), "子ノードの名前を取得できる");
            // メッシュを含むノードを検索します。
            Mesh foundMesh = null;
            for (int i = 0; i < rootNode.ChildCount; i++)
            {
                var child = rootNode.GetChildAt(i);
                var mesh = child.Mesh;
                if (mesh == null) continue;
                if (mesh.VerticesCount > 0 && mesh.IndicesCount >= 3)
                {
                    foundMesh = mesh;
                    break;
                }
            }

            return foundMesh;
        }
    }
}
