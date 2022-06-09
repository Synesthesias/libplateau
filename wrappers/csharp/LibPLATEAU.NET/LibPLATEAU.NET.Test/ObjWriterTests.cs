using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
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

            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);

            new ObjWriter(DllLogLevel.Trace).Write(objPath, cityModel, TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple));

            Assert.IsTrue(File.Exists(objPath));
        }

        [TestMethod]
        public void Dll_Log_Is_Sent_To_CSharp_StdOut()
        {
            var objWriter = new ObjWriter();
            var objPath = "53392642_bldg_6697_op2.obj";
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            
            // コンソール出力を奪い取って中身を確認できるようにします。
            var prevOut = Console.Out;
            var builder = new StringBuilder();
            var strWriter = new StringWriter(builder);
            Console.SetOut(strWriter);

            // ファイル変換でログが出てくることを期待します。
            // DLL内のログが C# の標準出力に転送されるはずです。
            objWriter.GetDllLogger().SetLogLevel(DllLogLevel.Info);
            objWriter.Write(objPath, cityModel, TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple));
            
            // ログが出ていることを確認します。
            var reader = new StringReader(builder.ToString());
            string consoleMessage = reader.ReadToEnd();
            StringAssert.Contains(consoleMessage, "Convert Start.");
            
            Console.SetOut(prevOut);
        }

        [TestMethod]
        public void GetMeshGranularity_Returns_The_Same_Value_As_Set()
        {
            var writer = new ObjWriter();
            writer.SetMeshGranularity(MeshGranularity.PerAtomicFeatureObject);
            Assert.AreEqual(writer.GetMeshGranularity(), MeshGranularity.PerAtomicFeatureObject);
            writer.SetMeshGranularity(MeshGranularity.PerCityModelArea);
            Assert.AreEqual(writer.GetMeshGranularity(), MeshGranularity.PerCityModelArea);
            writer.SetMeshGranularity(MeshGranularity.PerPrimaryFeatureObject);
            Assert.AreEqual(writer.GetMeshGranularity(), MeshGranularity.PerPrimaryFeatureObject);
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