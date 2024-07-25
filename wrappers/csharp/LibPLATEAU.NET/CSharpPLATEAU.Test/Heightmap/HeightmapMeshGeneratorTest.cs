using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geometries;
using PLATEAU.Heightmap;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.Heightmap
{
    [TestClass]
    public class HeightmapMeshGeneratorTest
    {
        [TestMethod]
        public void Flat_Heightmap_Is_Converted_To_Flat_Mesh()
        {
            // Arrange
            var generator = new HeightmapMeshGenerator();
            var mesh = Mesh.Create("");
            UInt16[] heightmapData = new UInt16[4] { 0, 0, 0, 0 };
            var min = new PlateauVector3d(0, 0, 0);
            var max = new PlateauVector3d(1, 1, 0);
            var minUV = new PlateauVector2f(0, 0);
            var maxUV = new PlateauVector2f(1, 1);

            // Act
            generator.Generate(ref mesh, 2, 2, 1, heightmapData, CoordinateSystem.ENU, min, max, minUV, maxUV, true);

            // Assert
            for (int i = 0; i < mesh.VerticesCount; i++)
            {
                Assert.AreEqual(0, mesh.GetVertexAt(i).Z);
            }

            mesh.Dispose();
        }
    }
}