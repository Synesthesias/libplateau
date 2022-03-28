using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjWriterTests
    {
        private const string gmlPath = "data/53392642_bldg_6697_op2.gml";

        [TestMethod]
        public void Write_Generates_Obj_File()
        {
            var objPath = "53392642_bldg_6697_op2.obj";
            var parserParams = new CitygmlParserParams
            {
                Optimize = 1
            };

            var cityModel = CityGml.Load(gmlPath, parserParams);

            new ObjWriter().Write(objPath, cityModel, gmlPath);

            Assert.IsTrue(System.IO.File.Exists(objPath));
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
            var cityModel = CityGml.Load(gmlPath, new CitygmlParserParams());
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