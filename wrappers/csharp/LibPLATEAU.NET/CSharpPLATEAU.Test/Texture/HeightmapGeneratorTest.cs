using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;
using PLATEAU.Texture;
using System;
using System.IO;

namespace PLATEAU.Test.Texture
{

    [TestClass]
    public class HeightmapGeneratorTest
    {
        public string ImageDirPath = "data/日本語パステスト/test_images";

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
        public void TestGenerateFromMesh()
        {
            SimpleMeshInfo(out var vertices, out var indices, out var uv1, out var uv4, out var subMeshes);
            var mesh = PLATEAU.PolygonMesh.Mesh.Create(vertices, indices, uv1, uv4, subMeshes);

            int textureWidth = 513;
            int textureHeight = 513;
            PlateauVector2d margin = new PlateauVector2d(0, 0);

            HeightmapGenerator gen = new HeightmapGenerator();
            gen.GenerateFromMesh(mesh, textureWidth, textureHeight, margin, true, out var min, out var max, out var minUV, out var maxUV, out var outData);
            Assert.AreEqual(textureWidth * textureHeight, outData.Length);
        }

        [TestMethod]
        public void TestReadWritePng()
        {
            var filePath = Path.Combine(ImageDirPath, "test_heightmap.png");
            Assert.IsTrue(Directory.Exists(ImageDirPath), "Directory Exists");
            Assert.IsTrue(File.Exists(filePath), "File Exists");

            //Read
            int textureWidth = 513;
            int textureHeight = 513;
            HeightmapGenerator.ReadPngFile(filePath, textureWidth, textureHeight, out var imageData);
            Assert.AreEqual(textureWidth * textureHeight, imageData.Length);

            //Write
            HeightmapGenerator.SavePngFile(Path.Combine(ImageDirPath, "test_heightmap_saved.png"), textureWidth, textureHeight, imageData);
        }

        [TestMethod]
        public void TestReadWriteRaw()
        {
            var filePath = Path.Combine(ImageDirPath, "test_heightmap.raw");
            Assert.IsTrue(Directory.Exists(ImageDirPath), "Directory Exists");
            Assert.IsTrue(File.Exists(filePath), "File Exists");

            //Read
            int textureWidth = 513;
            int textureHeight = 513;
            HeightmapGenerator.ReadRawFile(filePath, textureWidth, textureHeight, out var imageData);
            Assert.AreEqual(textureWidth * textureHeight, imageData.Length);

            //Write
            HeightmapGenerator.SaveRawFile(Path.Combine(ImageDirPath, "test_heightmap_saved.raw"), textureWidth, textureHeight, imageData);
        }


        private static PLATEAU.PolygonMesh.Mesh CreateSimpleMesh()
        {
            SimpleMeshInfo(out var vertices, out var indices, out var uv1, out var uv4, out var subMeshes);
            var mesh = PLATEAU.PolygonMesh.Mesh.Create(vertices, indices, uv1, uv4, subMeshes);
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
