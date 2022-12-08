using System;
using System.Diagnostics;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Geometries;
using PLATEAU.Interop;
using PLATEAU.Network;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class ServerDatasetAccessorTest
    {

        [TestMethod]
        public void GetMeshCodes_Returns_MeshCodes()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(3, meshCodes.Length);
            Assert.AreEqual("53392642", meshCodes.At(1).ToString());
        }

        [TestMethod]
        public void GetGmlFiles_Works()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            var gmlFiles = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, gmlFiles.Length);
            var gml = gmlFiles.At(0);
            Assert.AreEqual(NetworkConfig.DefaultApiServerUrl + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml", gml.Path);
        }

        [TestMethod]
        public void GetGmlFiles_Cache_Works()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            var stopwatch = Stopwatch.StartNew();
            var gmlFiles = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, gmlFiles.Length);
            stopwatch.Stop();
            var time1 = stopwatch.Elapsed;
            Console.WriteLine($"{time1} sec");
            stopwatch.Reset();
            stopwatch.Start();
            gmlFiles = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            stopwatch.Stop();
            var time2 = stopwatch.Elapsed;
            Console.WriteLine($"{time2} sec");
            Assert.AreEqual(2, gmlFiles.Length);
            Assert.IsTrue((time1 - time2).TotalMilliseconds > 500, "キャッシュにより、1回目より2回目のほうが速い（ネットワークアクセスが省略される）");
        }

        [TestMethod]
        public void GetPackages_Works()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            var buildings = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            var roads = accessor.GetGmlFiles(PredefinedCityModelPackage.Road);
            Assert.AreEqual(2, buildings.Length);
            var expected = PredefinedCityModelPackage.Building | PredefinedCityModelPackage.Road | PredefinedCityModelPackage.UrbanPlanningDecision | PredefinedCityModelPackage.LandUse;
            Assert.AreEqual(expected, accessor.Packages);
        }

        [TestMethod]
        public void MaxLod()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var accessor = source.Accessor;
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            var meshCode = gmls.At(0).MeshCode;
            Assert.AreEqual(2, gmls.At(1).GetMaxLod());
        }
        
        [TestMethod]
        public void CalcCenterPoint_Returns_Position_Of_Test_Data()
        {
            using var source = DatasetSource.Create(true, "23ku");
            var collection = source.Accessor;
            foreach (var meshCode in collection.MeshCodes)
            {
                var extent = meshCode.Extent;
                var min = extent.Min;
                var max = extent.Max;
                Console.WriteLine($"{min.Latitude}, {min.Longitude}, {min.Height}");
            }

            PlateauVector3d center;
            using (var geoRef = new GeoReference(new PlateauVector3d(0, 0, 0), 1.0f, CoordinateSystem.EUN, 9))
            {
                center = collection.CalculateCenterPoint(geoRef);
            }
            Console.WriteLine(center);
            // テスト用のデータは、基準点からおおむね南に51km, 西に5km の地点にあります。
            // ここでいう基準点とは、下のWebサイトにおける 9番の地点です。
            // https://www.gsi.go.jp/sokuchikijun/jpc.html
            Assert.IsTrue(Math.Abs(center.Z - (-51000)) < 1000, "南に51km");
            Assert.IsTrue(Math.Abs(center.X - (-5000)) < 1000, "西に5km");
        }
    }
}
