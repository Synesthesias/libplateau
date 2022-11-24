using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Interop;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class MeshCodeTest
    {
        [TestMethod]
        public void Get_Extent_Calculates_Returns_Proper_Coordinate()
        {
            var extent = MeshCode.Parse("53394525").Extent;
            var expected = new GeoCoordinate(35.68731814, 139.68926804, 0);

            AssertGE(extent.Max.Latitude, expected.Latitude);
            AssertGE(extent.Max.Longitude, expected.Longitude);
            AssertGE(expected.Latitude, extent.Min.Latitude);
            AssertGE(expected.Longitude, extent.Min.Longitude);
        }

        private static void AssertGE(double lhs, double rhs)
        {
            Assert.IsTrue(lhs >= rhs, $"{lhs} is not greater or equal to {rhs}");
        }
    }
}
