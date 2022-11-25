using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class TextureCoordinatesTests
    {
        private static CityModel cityModel;
        private static TextureCoordinates texCoords;
        private static LinearRing targetRing;

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // 探索して最初に見つかった TextureCoordinates をテスト対象にします。
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, false);
            var texTarget = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureTargetDefinitionsCount > 0)
                .Select(poly => poly.GetTextureTargetDefinition(0))
                .First(ttd => ttd.TexCoordinatesCount > 0);
            texCoords = texTarget.GetCoordinate(0);

            // texTarget のターゲットと同じIDを持つ LinearRing を探します。
            string targetRingId = texCoords.TargetLinearRingId;
            targetRing = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Select(poly => poly.ExteriorRing)
                .First(ring => ring.ID == targetRingId);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void Vec2CoordsCount_Is_Same_As_Linear_Ring_Vertices_Count()
        {
            int vec2CoordsCount = texCoords.Vec2CoordsCount;
            int ringVerticesCount = targetRing.VertexCount;
            Console.WriteLine($"Vec2CoordsCount = {vec2CoordsCount}");
            Console.WriteLine($"RingVerticesCount = {ringVerticesCount}");
            Assert.AreEqual(ringVerticesCount, vec2CoordsCount);
        }

        [TestMethod]
        public void GetVec2Coord_Is_Not_Zero()
        {
            var vec2 = texCoords.GetVec2Coord(0);
            Console.WriteLine($"GetVec2Coord(0) = {vec2}");
            Assert.IsTrue(vec2.IsNotZero());
        }

        [TestMethod]
        public void TargetLinearRingId_Returns_Not_Empty()
        {
            string id = texCoords.TargetLinearRingId;
            Console.WriteLine($"Target Ring ID : {id}");
            Assert.IsTrue(id.Length > 0);
        }

        [TestMethod]
        public void IsRingTarget_Returns_True_For_Target()
        {
            Console.WriteLine($"target ring id: {targetRing.ID}");
            bool actual = texCoords.IsRingTarget(targetRing);
            Assert.IsTrue(actual);
        }

    }
}
