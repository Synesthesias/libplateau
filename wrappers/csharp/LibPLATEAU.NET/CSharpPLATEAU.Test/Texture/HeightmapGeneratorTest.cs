using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;
using PLATEAU.Texture;

namespace PLATEAU.Test.Texture
{
    [TestClass]
    public class HeightmapGeneratorTest
    {
        [TestMethod]
        public void GenerateFromMeshTest()
        {
            
            SimpleMeshInfo(out var vertices, out var indices, out var uv1, out var uv4, out var subMeshes);
            var mesh = Mesh.Create(vertices, indices, uv1, uv4, subMeshes); 

            int TextureWidth = 513;
            int TextureHeight = 513;
            PlateauVector2d margin = new PlateauVector2d(0, 0);

            HeightmapGenerator Gen = new HeightmapGenerator();
            Gen.GenerateFromMesh(mesh, TextureWidth, TextureHeight, new PlateauVector2d(), out var Min, out var Max, out var outData );
            Assert.AreEqual(TextureWidth * TextureHeight, outData.Length);
        }

        private static Mesh CreateSimpleMesh()
        {
            SimpleMeshInfo(out var vertices, out var indices, out var uv1, out var uv4, out var subMeshes);
            var mesh = Mesh.Create(vertices, indices, uv1, uv4, subMeshes);
            return mesh;
        }

        private static void SimpleMeshInfo(out PlateauVector3d[] vertices, out uint[] indices,
            out PlateauVector2f[] uv1, out PlateauVector2f[] uv4, out SubMesh[] subMeshes)
        {
            vertices = new[]
            {
                new PlateauVector3d(0, 0, 0),
                new PlateauVector3d(0, 100, 10),
                new PlateauVector3d(100, 100, 30),
                new PlateauVector3d(100, 0, 10),
            };
            indices = new uint[]
            {
                0, 1, 2, 3, 2, 0
            };
            uv1 = new[]
            {
                new PlateauVector2f(1.1f, 1.2f),
                new PlateauVector2f(2.1f, 2.2f),
                new PlateauVector2f(3.1f, 3.2f)
            };
            uv4 = new[]
            {
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0),
                new PlateauVector2f(0, 0)
            };
            subMeshes = new[]
            {
                SubMesh.Create(0, 2, "testTexturePath.png")
            };
        }

    }
}
