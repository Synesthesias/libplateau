using System;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    /// <summary>
    /// <see cref="Polygon"/> のテストです。
    /// ただし <see cref="LinearRing"/> のテストは別途 <see cref="LinearRingTests"/> で行います。
    /// </summary>
    [TestClass]
    public class PolygonTests
    {
        private static CityModel cityModelWithTessellate;
        private static CityModel cityModelWithoutTessellate;
        private static Polygon polyWithVerts;
        private static Polygon polyWithInteriorRings;
        
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            cityModelWithTessellate = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            cityModelWithoutTessellate = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, true, false);

            // テスト対象として適切な Polygon を検索し、最初にヒットしたものをテストに利用します。
            // 具体的には VertexCount が 1以上である Polygon を探します。
            polyWithVerts = cityModelWithTessellate.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geo => geo.GeometryDescendantsDFS)
                .SelectMany(geo => geo.Polygons)
                .First(poly => poly.VertexCount > 0);

            polyWithInteriorRings = cityModelWithoutTessellate.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.Polygons)
                .First(poly => poly.InteriorRingCount > 0);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModelWithTessellate.Dispose();
            cityModelWithoutTessellate.Dispose();
        }

        [TestMethod]
        public void VertexCount_Returns_Positive_Value()
        {
            int actualVertCount = polyWithVerts.VertexCount;
            Console.WriteLine($"VertexCount: {actualVertCount}");
            Assert.IsTrue(actualVertCount > 0);
        }
        
        [TestMethod]
        public void GetVertex_Returns_Non_Zero_Position()
        {
            var vert = polyWithVerts.GetVertex(0);
            Console.WriteLine($"vertex: {vert}");
            Assert.IsTrue(vert.IsNotZero());
        }

        [TestMethod]
        public void IndicesCount_Returns_Positive_Value()
        {
            int indicesCount = polyWithVerts.IndicesCount;
            Console.WriteLine($"indices count: {indicesCount}");
            Assert.IsTrue(indicesCount > 0);
        }

        [TestMethod]
        public void Indices()
        {
            int[] indices = polyWithVerts.Indices.ToArray();
            Console.Write("indices: ");
            foreach (int idx in indices)
            {
                Console.Write($"{idx}, ");
            }
        }

        [TestMethod]
        public void InteriorRingsCount_Is_Same_As_Length_Of_InteriorRings()
        {
            int interiorRingsCount = polyWithInteriorRings.InteriorRingCount;
            int lengthOfInteriorRings = polyWithInteriorRings.InteriorRings.Count();
            Console.WriteLine($"Interior Rings Count: {interiorRingsCount}");
            Assert.AreEqual(interiorRingsCount, lengthOfInteriorRings);
        }

        [TestMethod]
        public void Do_Exist_InteriorRings_That_Have_Vertices_More_Than_Zero()
        {
            // 頂点数が1以上の InteriorRing を検索して1つでもあれば成功とします。
            var ring = cityModelWithoutTessellate.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.GeometryDescendantsDFS)
                .SelectMany(geom => geom.Polygons)
                .SelectMany(poly => poly.InteriorRings)
                .FirstOrDefault(r => r.VertexCount > 0);
            Assert.IsNotNull(ring);
        }

        [TestMethod]
        public void DoHaveVertices_Returns_Correct()
        {
            Assert.IsTrue(polyWithVerts.DoHaveVertices);
            Assert.IsFalse(polyWithInteriorRings.DoHaveVertices);
        }

        [TestMethod]
        public void DoHaveRings_Returns_Correct()
        {
            Assert.IsTrue(polyWithInteriorRings.DoHaveRings);
        }

        [TestMethod]
        public void GetVertex_Throws_Error_When_Out_Of_Range()
        {
            var poly = polyWithVerts;
            // 正常
            poly.GetVertex(poly.VertexCount - 1);
            // 異常
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => poly.GetVertex(poly.VertexCount));
        }
        
    }
}
