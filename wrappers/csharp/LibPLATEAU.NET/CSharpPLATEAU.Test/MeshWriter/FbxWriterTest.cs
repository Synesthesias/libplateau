using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.MeshWriter;
using PLATEAU.Test.CityGML;
using PLATEAU.Test.GeometryModel;

namespace PLATEAU.Test.MeshWriter
{
    [TestClass]
    public class FbxWriterTest
    {
        [TestMethod]
        public void WriteGenerateFbxFile()
        {
            string testDir = Path.GetFullPath("./testDir");
            if(Directory.Exists(testDir)) Directory.Delete(testDir, true);
            Directory.CreateDirectory(testDir);
            using var model = TestGeometryUtil.ExtractModel();
            string gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            string fbxFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".fbx";
            string fbxPath = Path.Combine(testDir, fbxFileName);
            var option = new FbxWriteOptions(FbxFileFormat.Binary);
            
            bool isSucceed = FbxWriter.Write(fbxPath, model, option);
            
            Assert.IsTrue(isSucceed);
            Assert.IsTrue(File.Exists(fbxPath), "fbxファイルが存在します。");
            Assert.IsTrue(new FileInfo(fbxPath).Length > 0, "fbxファイルの中身が1バイト以上存在します。");
            
            string expectedTextureDir =
                Path.Combine(testDir, "53392642_bldg_6697_appearance");
            Assert.IsTrue(Directory.Exists(expectedTextureDir), "テクスチャフォルダが存在します。");
            
            Directory.Delete(testDir, true);
        }
    }
}
