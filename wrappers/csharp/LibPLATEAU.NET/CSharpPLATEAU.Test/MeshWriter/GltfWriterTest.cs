using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Test.CityGML;
using PLATEAU.Test.GeometryModel;
using PLATEAU.MeshWriter;
using System.IO;

namespace PLATEAU.Test.MeshWriter
{
    [TestClass]
    public class GltfWriterTests
    {

        [TestMethod]
        public void Generates_Gltf_File_And_Check_Generated_File_Name()
        {
            using var model = TestGeometryUtil.ExtractModel();
            var gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            var gltfFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".glb";

            var option = new GltfWriteOptions(GltfFileFormat.GLB, "");//出力フォーマットとテクスチャファイルの保存先ディレクトリ名指定（NULLならディレクトリは作成しない）            

            var gltfWriter = new GltfWriter();
            if (option.GltfFileFormat == GltfFileFormat.GLTF) gltfFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".gltf";
            File.Delete(gltfFileName);
            var flg = gltfWriter.Write(gltfFileName, model, option);
            Assert.IsTrue(File.Exists(gltfFileName), "変換後ファイルが実在する");
            Assert.IsTrue(flg);
        }

    }
}