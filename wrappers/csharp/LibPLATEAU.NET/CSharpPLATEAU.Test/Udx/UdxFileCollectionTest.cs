using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geometries;
using PLATEAU.Interop;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class UdxFileCollectionTest
    {
        [TestMethod]
        public void Can_Get_All_Mesh_Codes()
        {
            var collection = UdxFileCollection.Find("data");
            Assert.AreEqual(1, collection.MeshCodes.Count);
        }

        [TestMethod]
        public void Filtered_Collection_Contains_All_Files_In_Extent()
        {
            var extent = new Extent
            {
                Max = new GeoCoordinate(35.53725521, 139.77699279, 0),
                Min = new GeoCoordinate(35.53725511, 139.77699239, 0)
            };
            var collection = UdxFileCollection.Find("data");
            var filteredCollection = collection.Filter(extent);
            Assert.AreEqual(1, filteredCollection.MeshCodes.Count);
            Assert.AreEqual("53392642", filteredCollection.MeshCodes[0].ToString());
        }

        [TestMethod]
        public void Packages_Getter_Returns_All_Packages()
        {
            var collection = UdxFileCollection.Find("data");
            var expected = PredefinedCityModelPackage.Building;
            Assert.AreEqual(expected, collection.Packages);
        }

        [TestMethod]
        public void Get_Gml_Files_Returns_All_Files()
        {
            var collection = UdxFileCollection.Find("data");
            Assert.AreEqual(Path.GetFullPath("data/udx/bldg/53392642_bldg_6697_op2.gml"), Path.GetFullPath(collection.GetGmlFiles(PredefinedCityModelPackage.Building)[0]));
        }

        [TestMethod]
        public void Fetch_Copies_Relative_Files()
        {
            var collection = UdxFileCollection.Find("data");
            var testDir = Directory.CreateDirectory("temp_test_dir");
            var gmlArray = collection.GetGmlFiles(PredefinedCityModelPackage.Building);
            foreach (var gml in gmlArray)
            {
                UdxFileCollection.Fetch(testDir.FullName, GmlFileInfo.Create(gml));
            }

            var shouldExists = new[]
            {
                "udx/bldg/53392642_bldg_6697_op2.gml",
                "udx/bldg/53392642_bldg_6697_appearance/hnap0034.tif",
                "codelists/Common_prefecture.xml"
            };
            foreach (var filePath in shouldExists)
            {
                var resultPath = "temp_test_dir/data/" + filePath;
                Assert.IsTrue(File.Exists(resultPath), $"{resultPath} does not exist");
            }
            Directory.Delete(testDir.FullName, true);
        }

        [TestMethod]
        public void FilterByMeshCodes_Contains_MeshCode_Only_If_Valid()
        {
            var collection = UdxFileCollection.Find("data");
            string validMeshCode = "53392642";
            string invalidMeshCode = "99999999";
            Assert.IsTrue(DoResultOfFilterByMeshCodesContainsMeshCode(collection, validMeshCode));
            Assert.IsFalse(DoResultOfFilterByMeshCodesContainsMeshCode(collection, invalidMeshCode));

        }

        [TestMethod]
        public void CalcCenterPoint_Returns_Position_Of_Test_Data()
        {
            var collection = UdxFileCollection.Find("data");
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
                center = collection.CalcCenterPoint(geoRef);
            }
            Console.WriteLine(center);
            // テスト用のデータは、基準点からおおむね南に51km, 西に5km の地点にあります。
            // ここでいう基準点とは、下のWebサイトにおける 9番の地点です。
            // https://www.gsi.go.jp/sokuchikijun/jpc.html
            Assert.IsTrue(Math.Abs(center.Z - (-51000)) < 1000, "南に51km");
            Assert.IsTrue(Math.Abs(center.X - (-5000)) < 1000, "西に5km");
        }

        private static bool DoResultOfFilterByMeshCodesContainsMeshCode(UdxFileCollection collection, string meshCode)
        {
            var filtered = collection.FilterByMeshCodes(new []{ MeshCode.Parse(meshCode)});
            var filteredGMLArray = filtered.GetGmlFiles(PredefinedCityModelPackage.Building);
            bool contains = false;
            foreach (var gml in filteredGMLArray)
            {
                if(gml.Contains(meshCode))
                {
                    contains = true;
                }
            }

            return contains;
        }
    }
}
