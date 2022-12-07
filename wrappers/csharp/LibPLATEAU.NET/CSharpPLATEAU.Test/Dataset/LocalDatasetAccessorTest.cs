using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;
using PLATEAU.Geometries;
using PLATEAU.Interop;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class LocalDatasetAccessorTest
    {
        [TestMethod]
        public void Can_Get_All_Mesh_Codes()
        {
            var source = DatasetSource.Create(false, "data");
            var accessor = source.Accessor;
            Assert.AreEqual(1, accessor.MeshCodes.Length);
        }

        [TestMethod]
        public void Filtered_Collection_Contains_All_Files_In_Extent()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmls.Length);
            Assert.AreEqual("53392642", gmls.At(0).MeshCode.ToString());
        }

        [TestMethod]
        public void Packages_Getter_Returns_All_Packages()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            var expected = PredefinedCityModelPackage.Building;
            Assert.AreEqual(expected, accessor.Packages);
        }

        [TestMethod]
        public void Get_Gml_Files_Returns_All_Files()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            Assert.AreEqual(
                Path.GetFullPath("data/udx/bldg/53392642_bldg_6697_op2.gml"),
                Path.GetFullPath(
                    accessor.GetGmlFiles(PredefinedCityModelPackage.Building)
                        .At(0).Path)
            );
        }

        [TestMethod]
        public void FilterByMeshCodes_Contains_MeshCode_Only_If_Valid()
        {
            var collection = LocalDatasetAccessor.Find("data");
            string validMeshCode = "53392642";
            string invalidMeshCode = "99999999";
            Assert.IsTrue(DoResultOfFilterByMeshCodesContainsMeshCode(collection, validMeshCode));
            Assert.IsFalse(DoResultOfFilterByMeshCodesContainsMeshCode(collection, invalidMeshCode));

        }

        [TestMethod]
        public void CalcCenterPoint_Returns_Position_Of_Test_Data()
        {
            var collection = LocalDatasetAccessor.Find("data");
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

        private static bool DoResultOfFilterByMeshCodesContainsMeshCode(LocalDatasetAccessor collection, string meshCode)
        {
            var filtered = collection.FilterByMeshCodes(new[] { MeshCode.Parse(meshCode) });
            var filteredGMLArray = filtered.GetGmlFiles(PredefinedCityModelPackage.Building);
            bool contains = false;
            foreach (var gml in filteredGMLArray)
            {
                if (gml.Contains(meshCode))
                {
                    contains = true;
                }
            }

            return contains;
        }
    }
}
