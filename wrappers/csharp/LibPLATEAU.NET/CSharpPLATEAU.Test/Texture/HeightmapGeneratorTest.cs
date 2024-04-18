using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;
using PLATEAU.Texture;
using System;

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
            Gen.GenerateFromMesh(mesh, TextureWidth, TextureHeight, margin, out var Min, out var Max, out var outData );
            Assert.AreEqual(TextureWidth * TextureHeight, outData.Length);
        }

        [TestMethod]
        public void ReadPngTest()
        {
            int TextureWidth = 513;
            int TextureHeight = 513;
            HeightmapGenerator.ReadPngFile("data/日本語パステスト/test_images/HeightMap.png", TextureWidth, TextureHeight, out var imageData);
            Assert.AreEqual(TextureWidth * TextureHeight, imageData.Length);
        }

        [TestMethod]
        public void ReadWriteRawTest()
        {
            //Read
            int TextureWidth = 513;
            int TextureHeight = 513;
            HeightmapGenerator.ReadRawFile("data/日本語パステスト/test_images/HeightMap.raw", TextureWidth, TextureHeight, out var imageData);
            Assert.AreEqual(TextureWidth * TextureHeight, imageData.Length);

            //Write
            HeightmapGenerator.SaveRawFile("data/日本語パステスト/test_images/HeightMapSaved.raw", TextureWidth, TextureHeight, imageData);
            HeightmapGenerator.SavePngFile("data/日本語パステスト/test_images/HeightMapSaved.png", TextureWidth, TextureHeight, imageData);
        }

        [TestMethod]
        public void ConvertTo2DFloatArrayTest()
        {
            ushort[] data = new ushort[4] { 65535, 0, 0, 65535 };
            float[,] outData = HeightmapGenerator.ConvertTo2DFloatArray(data, 2, 2);
            Assert.AreEqual(0f, outData[0, 0]);
            Assert.AreEqual(1f, outData[0, 1]);
            Assert.AreEqual(1f, outData[1, 0]);
            Assert.AreEqual(0f, outData[1, 1]);
        }

        [TestMethod]
        public void Debug()
        {
            HeightmapGenerator.Debug(1, "data/日本語パステスト/test_images/HeightMap.png", 513, 513, new PlateauVector2d(1,2), out var size, out var outVec);
            Assert.AreEqual(255, size);

            Assert.AreEqual(outVec.X, 1);
            Assert.AreEqual(outVec.Y, 2);
            Assert.AreEqual(outVec.Z, 3);

            Assert.IsTrue(true);
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
