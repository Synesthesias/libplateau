using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjWriterTests
    {
        [TestMethod]
        public void Write_Generates_Obj_File()
        {
            var objFileName = "53391540_bldg_6697_op.obj";
            new ObjWriter().Write(objFileName, "data/53391540_bldg_6697_op.gml");

            Assert.IsTrue(System.IO.File.Exists(objFileName));
        }

        [TestMethod]
        public void GetReferencePoint_Returns_The_Same_Value_As_Set()
        {
            var expectedValue = new PlateauVector3d(100000d, -30000d, 0.1d);
            var writer = new ObjWriter();
            writer.ReferencePoint = expectedValue;
            Assert.AreEqual(expectedValue, writer.ReferencePoint);
        }
    }
}