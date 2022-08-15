using System;
using System.Diagnostics;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.Interop;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class MeshMergerTests
    {
        private readonly MeshMerger meshMerger;

        public MeshMergerTests()
        {
            this.meshMerger = new MeshMerger();
        }

        [TestMethod]
        public void GridMerge_Returns_Polygons()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var logger = new DllLogger();
            logger.SetCallbacksToStdOut();
            var polygons = this.meshMerger.GridMerge(cityModel, CityObjectType.COT_All, 5, 5, logger);
            int numGridsWithVertices = polygons.Count(poly => poly.VertexCount > 0);
            Assert.IsTrue(numGridsWithVertices >= 10, "頂点を含むメッシュが10個以上ある");
        }

    }
}