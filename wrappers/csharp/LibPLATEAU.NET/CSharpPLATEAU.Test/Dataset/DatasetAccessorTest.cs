using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Geometries;
using PLATEAU.Interop;
using PLATEAU.Network;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class DatasetAccessorTest
    {

        [TestMethod]
        public void GetMeshCodesLocal()
        {
            using var source = DatasetSource.Create(false, "data", "");
            using var accessor = source.Accessor;
            Assert.AreEqual(1, accessor.MeshCodes.Length);
            Console.WriteLine(accessor.MeshCodes.At(0).ToString());
        }

        [TestMethod]
        public void GetMeshCodeServer()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(3, meshCodes.Length);
            Assert.AreEqual("53392642", meshCodes.At(1).ToString());
        }
        
        [TestMethod]
        public void GetGmlFilesLocal()
        {
            using var datasetSource = DatasetSource.Create(false, "data", "");
            using var accessor = datasetSource.Accessor;
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmls.Length);
            Assert.AreEqual("53392642", gmls.At(0).MeshCode.ToString());
            Assert.AreEqual(
                Path.GetFullPath("data/udx/bldg/53392642_bldg_6697_op2.gml"),
                Path.GetFullPath(gmls.At(0).Path)
            );
        }
        
        [TestMethod]
        public void GetGmlFilesServer()
        {
            using var source = DatasetSource.Create(true,  "", "23ku");
            using var accessor = source.Accessor;
            var gmlFiles = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, gmlFiles.Length);
            var gml = gmlFiles.At(0);
            Assert.AreEqual(NetworkConfig.DefaultApiServerUrl + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml", gml.Path);
        }
        
        [TestMethod]
        public void GetPackagesLocal()
        {
            using var datasetSource = DatasetSource.Create(false, "data", "");
            using var accessor = datasetSource.Accessor;
            Console.WriteLine(Path.GetFullPath(accessor.GetGmlFiles(PredefinedCityModelPackage.Building).At(0).Path));
            var expected = PredefinedCityModelPackage.Building;
            Assert.AreEqual(expected, accessor.Packages);
        }
        
        [TestMethod]
        public void GetPackagesServer()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
            var buildings = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, buildings.Length);
            var expected = PredefinedCityModelPackage.Building | PredefinedCityModelPackage.Road | PredefinedCityModelPackage.UrbanPlanningDecision | PredefinedCityModelPackage.LandUse;
            Assert.AreEqual(expected, accessor.Packages);
        }
        
        [TestMethod]
        public void CalcCenterLocal()
        {
            using var source = DatasetSource.Create(false, "data", "");
            TestCenterPoint(source);
        }

        [TestMethod]
        public void CalcCenterPointServer()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            TestCenterPoint(source);
        }

        [TestMethod]
        public void MaxLodLocal()
        {
            using var datasetSource = DatasetSource.Create(false, "data", "");
            using var accessor = datasetSource.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
            int maxLod = accessor.GetGmlFiles(PredefinedCityModelPackage.Building).At(0).GetMaxLod();
            Assert.AreEqual(2, maxLod);
        }
        
        [TestMethod]
        public void MaxLodServer()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, gmls.At(1).GetMaxLod());
        }
        
        [TestMethod]
        public void FilterByMeshCodes_Contains_MeshCode_Only_If_Valid_Local()
        {
            using var source = DatasetSource.Create(false, "data", "");
            using var accessor = source.Accessor;
            string validMeshCode = "53392642";
            string invalidMeshCode = "99999999";
            Assert.IsTrue(DoResultOfFilterByMeshCodesContainsMeshCode(accessor, validMeshCode));
            Assert.IsFalse(DoResultOfFilterByMeshCodesContainsMeshCode(accessor, invalidMeshCode));
        }

        [TestMethod]
        public void FilterByMeshCodes_Contains_MeshCode_Only_If_Valid_Server()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
            string validMeshCode = "53392642";
            string invalidMeshCode = "99999999";
            Assert.IsTrue(DoResultOfFilterByMeshCodesContainsMeshCode(accessor, validMeshCode));
            Assert.IsFalse(DoResultOfFilterByMeshCodesContainsMeshCode(accessor, invalidMeshCode));
        }
        
        [TestMethod]
        public void GetGmlFiles_Server_Cache_Works()
        {
            using var source = DatasetSource.Create(true, "", "23ku");
            using var accessor = source.Accessor;
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
            Assert.IsTrue(time2.TotalMilliseconds < time1.TotalMilliseconds * 0.7, "キャッシュにより、1回目より2回目のほうが速い（ネットワークアクセスが省略される）");
        }


        private static void TestCenterPoint(DatasetSource source)
        {
            using var collection = source.Accessor;
            var meshCodes = collection.MeshCodes.ToArray();
            Assert.IsTrue(meshCodes.Length > 0);
            foreach (var meshCode in meshCodes)
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
        
        
        private static bool DoResultOfFilterByMeshCodesContainsMeshCode(DatasetAccessor accessor, string meshCodeStr)
        {
            using var filtered = accessor.FilterByMeshCodes(new[] { MeshCode.Parse(meshCodeStr) });
            var filteredGMLArray = filtered.GetGmlFiles(PredefinedCityModelPackage.Building);
            bool contains = false;
            foreach (var gml in filteredGMLArray)
            {
                var meshCode = gml.MeshCode;
                if (meshCode.ToString() == meshCodeStr)
                {
                    contains = true;
                    break;
                }
            }
            return contains;
        }
    }
}
