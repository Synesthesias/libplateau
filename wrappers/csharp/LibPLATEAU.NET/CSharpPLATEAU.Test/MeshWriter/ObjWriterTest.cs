using Microsoft.VisualStudio.TestTools.UnitTesting;
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
            using var model = TestGeometryUtil.ExtractModel();
            var gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            var objFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".obj";
            var expectedObjFileName = Path.GetFileNameWithoutExtension(gmlPath) + "_LOD2.obj";

            File.Delete(expectedObjFileName);
            new ObjWriter().Write(objFileName, model);
            Assert.IsTrue(File.Exists(expectedObjFileName), $"{expectedObjFileName} does not exist.");
        }

    }
}