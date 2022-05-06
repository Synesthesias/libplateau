using System;
using System.Collections.Generic;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class LinearRingTests
    {
        private LinearRing exteriorRing;
        // 前処理
        public LinearRingTests()
        {
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile(1, false);
            
            // ExteriorRings を検索します。最初に見つかったリングをテストに利用します。 
            var exteriorRings = new List<LinearRing>();
            foreach (var co in cityModel.RootCityObjects)
            {
                foreach (var g in co.Geometries)
                {
                    FindExteriorRing(g, exteriorRings);
                }
            }
            Console.WriteLine($"Found ExteriorRings Count: {exteriorRings.Count}");
            this.exteriorRing = exteriorRings[0];
        }
        

        [TestMethod]
        public void ExteriorRing_Is_Not_Null()
        {
            Console.WriteLine(this.exteriorRing.Handle.ToInt64());
            Assert.IsNotNull(this.exteriorRing);
        }
        
        [TestMethod]
        public void VerticesCount_Returns_Positive_Number()
        {
            int verticesCount = this.exteriorRing.VerticesCount;
            Console.WriteLine($"Vertices Count: {verticesCount}");
            Assert.IsTrue(verticesCount > 0);
        }

        [TestMethod]
        public void GetVertex_Returns_Non_Zero()
        {
            var vert = this.exteriorRing.GetVertex(this.exteriorRing.VerticesCount - 1);
            Console.WriteLine($"Vertex: {vert}");
            bool isNonZero = true;
            isNonZero &= Math.Abs(vert.X) > 0.01;
            isNonZero &= Math.Abs(vert.Y) > 0.01;
            isNonZero &= Math.Abs(vert.Z) > 0.01;
            Assert.IsTrue(isNonZero);
        }
        

        private void FindExteriorRing(Geometry geom, List<LinearRing> found)
        {
            foreach (var p in geom.Polygons)
            {
                var ring = p.ExteriorRing;
                if (ring.VerticesCount > 0)
                {
                    found.Add(ring);
                }
            }

            foreach (var g in geom.ChildGeometries)
            {
                FindExteriorRing(g,　found);
            }
        }
    }
}