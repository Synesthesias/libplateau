using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjWriterTests
    {
        [TestMethod]
        public void Write_Generates_Obj_File()
        {
            var objPath = "53392642_bldg_6697_op2.obj";
            var parserParams = new CitygmlParserParams
            {
                Optimize = true
            };

            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);

            new ObjWriter().Write(objPath, cityModel, TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple));

            Assert.IsTrue(System.IO.File.Exists(objPath));
        }

        [TestMethod]
        public void GetMergeMeshFlg_Returns_The_Same_Value_As_Set()
        {
            var writer = new ObjWriter();
            writer.SetMergeMeshFlg(true);
            Assert.AreEqual(writer.GetMergeMeshFlg(), true);
            writer.SetMergeMeshFlg(false);
            Assert.AreEqual(writer.GetMergeMeshFlg(), false);
        }

        [TestMethod]
        public void GetDestAxes_Returns_The_Same_Value_As_Set()
        {
            var writer = new ObjWriter();
            writer.SetDestAxes(AxesConversion.RUF);
            Assert.AreEqual(writer.GetDestAxes(), AxesConversion.RUF);
            writer.SetDestAxes(AxesConversion.WNU);
            Assert.AreEqual(writer.GetDestAxes(), AxesConversion.WNU);
        }

        [TestMethod]
        public void GetReferencePoint_Returns_The_Same_Value_As_Set()
        {
            var expectedValue = new PlateauVector3d(100000d, -30000d, 0.1d);
            var writer = new ObjWriter();
            writer.ReferencePoint = expectedValue;
            AreEqual(expectedValue, writer.ReferencePoint);
        }

        [TestMethod]
        public void SetValidReferencePoint_Sets_As_Center_Of_Envelope()
        {
            var expectedValue = new PlateauVector3d(
                -5099.63214d,
                -51025.16812d,
                2.466d);
            var writer = new ObjWriter();
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            writer.SetValidReferencePoint(cityModel);
            AreEqual(expectedValue, writer.ReferencePoint);
        }

        private static void AreEqual(PlateauVector3d expected, PlateauVector3d actual)
        {
            Assert.AreEqual(expected.X, actual.X, 0.001d);
            Assert.AreEqual(expected.Y, actual.Y, 0.001d);
            Assert.AreEqual(expected.Z, actual.Z, 0.001d);
        }
    }
}