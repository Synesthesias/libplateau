using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.GeometryModel
{
    [TestClass]
    public class NodeTests
    {
        [TestMethod]
        public void CreateNode_Then_Dispose_Then_Invalid()
        {
            var node = Node.Create("testNode");
            Assert.AreEqual("testNode", node.Name, "Createできる");
            node.Dispose();
            Assert.ThrowsException<Exception>(() => node.Name, "Dispose後は情報にアクセスできない");
        }

        [TestMethod]
        public void Move_To_Model_Then_Node_Is_Invalid()
        {
            var node = Node.Create("testNode");
            Assert.AreEqual("testNode", node.Name);
            var model = Model.Create();
            model.AddNodeByCppMove(node);
            Assert.ThrowsException<Exception>(() => node.Name, "Modelにmoveしたあと、Nodeの情報にアクセスできない");
        }

        [TestMethod]
        public void Set_Mesh_Then_Mesh_Is_Invalid()
        {
            var node = Node.Create("testNode");
            var mesh = Mesh.Create("testMesh");
            Assert.AreEqual(0, mesh.VerticesCount);
            node.SetMeshByCppMove(mesh);
            Assert.ThrowsException<Exception>(() => mesh.VerticesCount, "MeshをMoveしたあと、Meshの情報にアクセスできない");
        }

        [TestMethod]
        public void AddChildNode()
        {
            var parent = Node.Create("parent");
            var child = Node.Create("child");
            parent.AddChildNodeByCppMove(child);
            Assert.AreEqual("child", parent.GetChildAt(0).Name, "子が追加されます。");
            Assert.ThrowsException<Exception>(() => child.Name, "move後は利用不可になります。");
        }
    }
}
