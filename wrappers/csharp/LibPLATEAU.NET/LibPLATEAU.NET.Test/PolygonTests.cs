using System;
using System.Linq;
using System.Runtime;
using LibPLATEAU.NET.CityGML;
using LibPLATEAU.NET.Util;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    /// <summary>
    /// <see cref="Polygon"/> のテストです。
    /// ただし <see cref="LinearRing"/> のテストは別途 <see cref="LinearRingTests"/> で行います。
    /// </summary>
    [TestClass]
    public class PolygonTests
    {
        private Polygon polyWithVerts;

        private Polygon polyWithInteriorRings;
        // 前処理
        public PolygonTests()
        {
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile(TestGMLLoader.GmlFileCase.Simple);

            // テスト対象として適切な Polygon を検索し、最初にヒットしたものをテストに利用します。
            // 具体的には VertexCount が 1以上である Polygon を探します。
            var allCityObjects = cityModel.RootCityObjects.SelectMany(co => co.ChildrenDfsIterator).ToArray();
            this.polyWithVerts = allCityObjects.SelectMany(co =>
                co.Geometries.SelectMany(geo => geo.Polygons.Where(poly => poly.VertexCount > 0))).First();

            this.polyWithInteriorRings = allCityObjects
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.Polygons)
                .First(poly => poly.InteriorRingCount > 0);
        }

        [TestMethod]
        public void VertexCount_Returns_Positive_Value()
        {
            int actualVertCount = this.polyWithVerts.VertexCount;
            Console.WriteLine($"VertexCount: {actualVertCount}");
            Assert.IsTrue(actualVertCount > 0);
        }
        

        [TestMethod]
        public void GetVertex_Returns_Non_Zero_Position()
        {
            var vert = this.polyWithVerts.GetVertex(0);
            Console.WriteLine($"vertex: {vert}");
            Assert.IsTrue(Math.Abs(vert.X) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Y) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Z) > 0.001);
        }

        [TestMethod]
        public void IndicesCount_Returns_Positive_Value()
        {
            int indicesCount = this.polyWithVerts.IndicesCount;
            Console.WriteLine($"indices count: {indicesCount}");
            Assert.IsTrue(indicesCount > 0);
        }

        [TestMethod]
        public void Indices()
        {
            int[] indices = this.polyWithVerts.Indices.ToArray();
            Console.Write("indices: ");
            foreach (int idx in indices)
            {
                Console.Write($"{idx}, ");
            }
        }

        [TestMethod]
        public void InteriorRingsCount_Is_Same_As_Length_Of_InteriorRings()
        {
            int interiorRingsCount = this.polyWithInteriorRings.InteriorRingCount;
            int lengthOfInteriorRings = this.polyWithInteriorRings.InteriorRings.Count();
            Console.WriteLine($"Interior Rings Count: {interiorRingsCount}");
            Assert.AreEqual(interiorRingsCount, lengthOfInteriorRings);
        }
    }
}