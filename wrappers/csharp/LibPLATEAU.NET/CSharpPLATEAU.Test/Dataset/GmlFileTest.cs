﻿using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class GmlFileTest
    {
        [TestMethod]
        public void GetPath_Returns_Path()
        {
            const string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual(path, info.Path);
            info.Dispose();
        }

        [TestMethod]
        public void FeatureType()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual("bldg", info.FeatureType);
            info.Dispose();
        }

        [TestMethod]
        public void TestMeshCode()
        {
            string path = "data/udx/bldg/53392642_bldg_6697_op2.gml";
            var info = GmlFile.Create(path);
            Assert.AreEqual("53392642", info.MeshCode.ToString());
            info.Dispose();
        }
        
        [TestMethod]
        public void DirNameToPackage_Returns_Gml_Package()
        {
            var gmlInfo = GmlFile.Create("foobar/udx/bldg/53392546_bldg_6697_2_op.gml");
            Assert.AreEqual(PredefinedCityModelPackage.Building, gmlInfo.Package);
            gmlInfo.Dispose();
        }
        
        [TestMethod]
        public void Fetch_Local_Copies_Relative_Files()
        {
            using var source = DatasetSource.Create(false, "data");
            using var accessor = source.Accessor;
            var testDir = Directory.CreateDirectory("temp_test_dir");
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            foreach (var gml in gmls)
            {
                gml.Fetch(testDir.FullName);
            }

            string[] shouldExists = 
            {
                "udx/bldg/53392642_bldg_6697_op2.gml",
                "udx/bldg/53392642_bldg_6697_appearance/hnap0034.tif",
                "codelists/Common_prefecture.xml"
            };
            foreach (string filePath in shouldExists)
            {
                string resultPath = "temp_test_dir/data/" + filePath;
                Assert.IsTrue(File.Exists(resultPath), $"{resultPath} does not exist");
            }
            Directory.Delete(testDir.FullName, true);
        }

        [TestMethod]
        public void GetMaxLod()
        {
            // TODO Serverのときは取得できるのか？
            using var source = DatasetSource.Create(false, "data");
            using var accessor = source.Accessor;
            var filtered = accessor.FilterByMeshCodes(new [] { MeshCode.Parse("53392642") });
            var gmls = filtered.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmls.Length);
            Assert.AreEqual(2, gmls.At(0).GetMaxLod());

            // TODO dispose filtered
        }

        // TODO あとで消す
        [TestMethod]
        public void SearchCodelistPathsAndTexturePaths()
        {
            using var sourceServer = DatasetSource.Create(false, "data");
            using var sourceLocal = DatasetSource.Create(true, "23ku");
            
            FetchAndCheckFiles(sourceServer);
            FetchAndCheckFiles(sourceLocal);
            
            
            void FetchAndCheckFiles(DatasetSource datasetSource){
                using var accessor = datasetSource.Accessor;
                var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
                var gml = gmls.At(0);
                var codelistPaths = gml.SearchAllCodelistPathsInGml().ToCSharpArray();
                var imagePaths = gml.SearchAllImagePathsInGml().ToCSharpArray();
                var expectedCodelists = new[]
                {
                    "../../codelists/Common_districtsAndZonesType.xml",
                    "../../codelists/Common_localPublicAuthorities.xml",
                    "../../codelists/Common_prefecture.xml",
                    "../../codelists/extendedAttribute_key.xml",
                    "../../codelists/extendedAttribute_key106.xml",
                    "../../codelists/extendedAttribute_key2.xml"
                };
                var expectedImages = new[]
                {
                    "53392642_bldg_6697_appearance/hnap0034.tif",
                    "53392642_bldg_6697_appearance/hnap0275.tif",
                    "53392642_bldg_6697_appearance/hnap0276.tif",
                    "53392642_bldg_6697_appearance/hnap0279.tif",
                    "53392642_bldg_6697_appearance/hnap0285.tif",
                    "53392642_bldg_6697_appearance/hnap0286.tif",
                    "53392642_bldg_6697_appearance/hnap0876.tif",
                    "53392642_bldg_6697_appearance/hnap0878.tif"
                };
                CollectionAssert.AreEquivalent(expectedCodelists, codelistPaths);
                CollectionAssert.AreEquivalent(expectedImages, imagePaths);}
            
        }

        [TestMethod]
        public void Fetch_Server_Downloads_Files()
        {
            using var source = DatasetSource.Create(new DatasetSourceConfig(true, "23ku"));
            using var accessor = source.Accessor;
            var testDir = Directory.CreateDirectory("temp_test_dir");
            
            if(Directory.Exists(testDir.FullName)) Directory.Delete(testDir.FullName, true);
            
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Console.WriteLine(testDir.FullName);
            var fetchedGml = gmls.At(0).Fetch(testDir.FullName);
            bool textureExist = File.Exists(Path.Combine(testDir.FullName,
                "13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_appearance/hnap0034.jpg"));
            bool codelistExist = File.Exists(Path.Combine(testDir.FullName,
                "13100_tokyo23-ku_2020_citygml_3_2_op/codelists/Common_prefecture.xml"));
            Assert.IsTrue(textureExist);
            Assert.IsTrue(codelistExist);

            Directory.Delete(testDir.FullName, true);
        }
    }
}
