﻿using System;
using System.Linq;
using System.Reflection.Metadata;
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
        private CityModel cityModelWithoutTesselate;
        private Polygon polyWithVerts;

        private Polygon polyWithInteriorRings;
        // 前処理
        public PolygonTests()
        {
            CityModel cityModelWithTesselate = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, 1, true);
            this.cityModelWithoutTesselate = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple, 1, false);

            // テスト対象として適切な Polygon を検索し、最初にヒットしたものをテストに利用します。
            // 具体的には VertexCount が 1以上である Polygon を探します。
            this.polyWithVerts = cityModelWithTesselate.RootCityObjects
                .SelectMany(co => co.ChildCityObjectsDfsIterate)
                .SelectMany(co => co.Geometries)
                .SelectMany(geo => geo.ChildGeometriesDfsIterate)
                .SelectMany(geo => geo.Polygons)
                .First(poly => poly.VertexCount > 0);
            

            this.polyWithInteriorRings = this.cityModelWithoutTesselate.RootCityObjects
                .SelectMany(co => co.ChildCityObjectsDfsIterate)
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
            Assert.IsTrue(vert.IsNotZero());
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

        [TestMethod]
        public void Do_Exist_InteriorRings_That_Have_Vertices_More_Than_Zero()
        {
            var ring = this.cityModelWithoutTesselate.RootCityObjects
                .SelectMany(co => co.ChildCityObjectsDfsIterate)
                .SelectMany(co => co.Geometries)
                .SelectMany(geom => geom.ChildGeometriesDfsIterate)
                .SelectMany(geom => geom.Polygons)
                .SelectMany(poly => poly.InteriorRings)
                .FirstOrDefault(ring => ring.VerticesCount > 0);
            Assert.IsNotNull(ring);
        }
        
    }
}