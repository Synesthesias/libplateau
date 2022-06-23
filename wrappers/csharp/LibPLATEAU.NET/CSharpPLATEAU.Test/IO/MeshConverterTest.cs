using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.IO;
using PLATEAU.Test.CityGML;
using System.IO;
using System.Text;

namespace PLATEAU.Test.IO
{
    [TestClass]
    public class MeshConverterTest
    {
        [TestMethod]
        public void Write_Generates_Obj_File_For_Every_LOD()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);
            new MeshConverter().Convert(".", TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple), cityModel);
            var objFileName = Path.GetFileNameWithoutExtension(gmlPath) + ".obj";
            Assert.IsTrue(File.Exists($"LOD0_{objFileName}"));
            Assert.IsTrue(File.Exists($"LOD1_{objFileName}"));
            Assert.IsTrue(File.Exists($"LOD2_{objFileName}"));
        }

        [TestMethod]
        public void Dll_Log_Is_Sent_To_CSharp_StdOut()
        {
            var converter = new MeshConverter();
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var gmlPath = TestUtil.GetGmlPath(TestUtil.GmlFileCase.Simple);

            // コンソール出力を奪い取って中身を確認できるようにします。
            var prevOut = Console.Out;
            var builder = new StringBuilder();
            var strWriter = new StringWriter(builder);
            Console.SetOut(strWriter);

            // ファイル変換でログが出てくることを期待します。
            // DLL内のログが C# の標準出力に転送されるはずです。
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            logger.SetLogLevel(DllLogLevel.Info);
            converter.Convert(".", gmlPath, cityModel, logger);

            // ログが出ていることを確認します。
            var reader = new StringReader(builder.ToString());
            string consoleMessage = reader.ReadToEnd();
            StringAssert.Contains(consoleMessage, "Start conversion.");

            Console.SetOut(prevOut);
        }

        [TestMethod]
        public void GetOptions_Returns_The_Same_Value_As_Set()
        {
            var converter = new MeshConverter();
            var options = new MeshConvertOptions();
            options.MeshGranularity = MeshGranularity.PerAtomicFeatureObject;
            converter.Options = options;
            Assert.AreEqual(converter.Options.MeshGranularity, MeshGranularity.PerAtomicFeatureObject);
            options.MeshGranularity = MeshGranularity.PerCityModelArea;
            converter.Options = options;
            Assert.AreEqual(converter.Options.MeshGranularity, MeshGranularity.PerCityModelArea);
            options.MeshGranularity = MeshGranularity.PerPrimaryFeatureObject;
            converter.Options = options;
            Assert.AreEqual(converter.Options.MeshGranularity, MeshGranularity.PerPrimaryFeatureObject);

            var expectedValue = new PlateauVector3d(
                -5099.63214d,
                -51025.16812d,
                2.466d);
            converter.Options.ReferencePoint = expectedValue;
            AreEqual(expectedValue, converter.Options.ReferencePoint);
        }

        private static void AreEqual(PlateauVector3d expected, PlateauVector3d actual)
        {
            Assert.AreEqual(expected.X, actual.X, 0.001d);
            Assert.AreEqual(expected.Y, actual.Y, 0.001d);
            Assert.AreEqual(expected.Z, actual.Z, 0.001d);
        }
    }
}