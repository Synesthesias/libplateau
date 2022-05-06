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
        private Polygon polygon;
        // 前処理
        public PolygonTests()
        {
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile();
            this.polygon = null;
            // Polygon を検索し、最初に見つかったものをテスト対象にします。
            // foreach (var co in cityModel.RootCityObjects)
            // {
            //     foreach (var geo in co.Geometries)
            //     {
            //         if (geo.PolygonCount > 0)
            //         {
            //             this.polygon = geo.GetPolygon(0);
            //             break;
            //         }
            //     }
            // }
            // this.polygon = cityModel.CityObjectsWhere(cityObject =>
            // {
            //     return cityObject.GeometriesWhere(geo =>
            //         geo.Polygons.FirstOrDefault(p => p.VertexCount > 0) != default ) != default;
            // }).First().GetGeometry(0).Polygons.First(p => p.VertexCount > 0);
            this.polygon = cityModel.RootCityObjects.SelectMany(co=>co.IterateChildrenDfs()).SelectMany(co =>
                co.Geometries.SelectMany(geo => geo.Polygons.Where(poly => poly.VertexCount >= 0))).First();

            if (this.polygon == null)
            {
                throw new Exception($"{nameof(Polygon)} is not found.");
            }
        }

        [TestMethod]
        public void VertexCount_Returns_Positive_Value()
        {
            int actualVertCount = this.polygon.VertexCount;
            Console.WriteLine($"VertexCount: {actualVertCount}");
            Assert.IsTrue(actualVertCount > 0);
        }
        

        [TestMethod]
        public void GetVertex_Returns_Non_Zero_Position()
        {
            var vert = this.polygon.GetVertex(0);
            Console.WriteLine($"vertex: {vert}");
            Assert.IsTrue(Math.Abs(vert.X) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Y) > 0.001);
            Assert.IsTrue(Math.Abs(vert.Z) > 0.001);
        }

        [TestMethod]
        public void IndicesCount_Returns_Positive_Value()
        {
            int indicesCount = this.polygon.IndicesCount;
            Console.WriteLine($"indices count: {indicesCount}");
            Assert.IsTrue(indicesCount > 0);
        }

        [TestMethod]
        public void Indices()
        {
            int[] indices = this.polygon.Indices.ToArray();
            Console.Write("indices: ");
            foreach (int idx in indices)
            {
                Console.Write($"{idx}, ");
            }
        }
    }
}