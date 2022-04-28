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
            // TODO 要確認 GMLのパース時に tesselate を false にしたときに限り、 Polygon の Vertices, Indices の代わりに  externalRing, innerRing に変わることがある？
            CityModel cityModel = TestGMLLoader.LoadTestGMLFile(1, false);
            CityObject cityObject = cityModel.RootCityObjects[0];


            
            // TODO この下の処理で、頂点数が 1以上である ExteriorRing を検索し、それをテスト対象にしようとしましたが、該当数が 0 であるためテストできていません。
            
            var exteriorRings = new List<LinearRing>();
            foreach (var co in cityModel.RootCityObjects)
            {
                Console.WriteLine($"Implicit Geometry Count:  {co.ImplicitGeometryCount}");
                foreach (var g in co.Geometries)
                {
                    Console.WriteLine(g.Type);
                    FindExteriorRing(g, exteriorRings);
                }
            }
            Console.WriteLine($"Found ExteriorRings Count: {exteriorRings.Count}");
            // this.exteriorRing = exteriorRings[0];
        }
        

        // [TestMethod]
        // public void ExteriorRing_Is_Not_Null()
        // {
        //     Console.WriteLine(this.exteriorRing.Handle.ToInt64());
        //     Assert.IsNotNull(this.exteriorRing);
        // }
        //
        // [TestMethod]
        // public void VerticesCount()
        // {
        //     int verticesCount = this.exteriorRing.VerticesCount;
        //     Console.WriteLine(verticesCount);
        // }

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