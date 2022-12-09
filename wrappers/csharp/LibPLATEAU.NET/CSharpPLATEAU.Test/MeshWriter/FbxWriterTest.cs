using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.MeshWriter;
using PLATEAU.Test.CityGML;
using PLATEAU.Test.GeometryModel;

namespace PLATEAU.Test.MeshWriter
{
    [TestClass]
    public class FbxWriterTest
    {
        [TestMethod]
        public void WriteGenerateAFile()
        {
            using var model = TestGeometryUtil.ExtractModel();
            string gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            string fbxFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".fbx";
            string fbxPath = new DirectoryInfo(gmlPath).Parent?.FullName + fbxFileName;
            var option = new FbxWriteOptions(FbxFileFormat.Binary);
            
            File.Delete(fbxPath);
            bool isSucceed = FbxWriter.Write(fbxPath, model, option);
            
            Assert.IsTrue(isSucceed);
            Assert.IsTrue(File.Exists(fbxPath), "fbxファイルが存在します。");
            Assert.IsTrue(new FileInfo(fbxPath).Length > 0, "fbxファイルの中身が1バイト以上存在します。");
            Console.WriteLine(fbxPath);
            File.Delete(fbxPath);
        }
    }
}
