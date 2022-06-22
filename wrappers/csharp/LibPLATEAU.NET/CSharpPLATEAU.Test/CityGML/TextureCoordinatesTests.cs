using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{

    [TestClass]
    public class TextureCoordinatesTests
    {
        private TextureCoordinates texCoords;
        private LinearRing targetRing;

        // 前準備
        public TextureCoordinatesTests()
        {
            // 探索して最初に見つかった TextureCoordinates をテスト対象にします。
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, false);
            var texTarget = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Where(poly => poly.TextureTargetDefinitionsCount > 0)
                .Select(poly => poly.GetTextureTargetDefinition(0))
                .First(ttd => ttd.TexCoordinatesCount > 0);
            this.texCoords = texTarget.GetCoordinate(0);

            // texTarget のターゲットと同じIDを持つ LinearRing を探します。
            string targetRingId = this.texCoords.TargetLinearRingId;
            this.targetRing = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .Select(poly => poly.ExteriorRing)
                .First(ring => ring.ID == targetRingId);
        }

        [TestMethod]
        public void Vec2CoordsCount_Is_Same_As_Linear_Ring_Vertices_Count()
        {
            int vec2CoordsCount = this.texCoords.Vec2CoordsCount;
            int ringVerticesCount = this.targetRing.VertexCount;
            Console.WriteLine($"Vec2CoordsCount = {vec2CoordsCount}");
            Console.WriteLine($"RingVerticesCount = {ringVerticesCount}");
            Assert.AreEqual(ringVerticesCount, vec2CoordsCount);
        }

        [TestMethod]
        public void GetVec2Coord_Is_Not_Zero()
        {
            var vec2 = this.texCoords.GetVec2Coord(0);
            Console.WriteLine($"GetVec2Coord(0) = {vec2}");
            Assert.IsTrue(vec2.IsNotZero());
        }

        [TestMethod]
        public void TargetLinearRingId_Returns_Not_Empty()
        {
            string id = this.texCoords.TargetLinearRingId;
            Console.WriteLine($"Target Ring ID : {id}");
            Assert.IsTrue(id.Length > 0);
        }

        [TestMethod]
        public void IsRingTarget_Returns_True_For_Target()
        {
            Console.WriteLine($"target ring id: {this.targetRing.ID}");
            bool actual = this.texCoords.IsRingTarget(this.targetRing);
            Assert.IsTrue(actual);
        }

    }
}