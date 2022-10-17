using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Test.CityGML;
using PLATEAU.Test.GeometryModel;
using PLATEAU.MeshWriter;
using System.IO;

namespace PLATEAU.Test.MeshWriter
{
    [TestClass]
    public class ObjWriterTests
    {

        [TestMethod]
        public void Generates_Obj_File_And_Check_Generated_File_Name()
        {
            var model = TestGeometryUtil.ExtractModel();
            var gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            var objFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".obj";

            var obj_writer = new ObjWriter();
            File.Delete(objFileName);
            var flg = obj_writer.Write(objFileName, model);
            Assert.IsTrue(File.Exists(objFileName), "変換後ファイルが実在する");
        }

    }
}