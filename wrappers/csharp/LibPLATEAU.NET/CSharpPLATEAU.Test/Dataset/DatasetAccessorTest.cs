﻿using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Dataset;

namespace PLATEAU.Test.Dataset
{
    [TestClass]
    public class DatasetAccessorTest
    {
        [TestMethod]
        public void TestGetGmlFiles()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            var gmls = accessor.GetGmlFiles(PredefinedCityModelPackage.Building);
            Assert.AreEqual(1, gmls.Length);
        }

        [TestMethod]
        public void TestMeshCodesGetter()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
        }
        
        [TestMethod]
        public void TestGetMaxLod()
        {
            using var datasetSource = DatasetSource.Create(false, "data");
            var accessor = datasetSource.Accessor;
            var meshCodes = accessor.MeshCodes;
            Assert.AreEqual(1, meshCodes.Length);
            int maxLod = accessor.GetMaxLod(meshCodes.At(0), PredefinedCityModelPackage.Building);
            Assert.AreEqual(2, maxLod);
        }
        
    }
}
